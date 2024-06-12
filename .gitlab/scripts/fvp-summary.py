#! /usr/bin/env python3
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

import argparse
import os
from artifactory import ArtifactoryBuildManager
import pandas as pd
import jinja2
import pathlib
import datetime as dt
import requests


class ArtifactoryHandler(object):
    def __init__(self):
        password = self._getenv("FVP_SERVER_KEY")
        user = self._getenv("FVP_SERVER_USER")
        self.artifactory_url = self._getenv("FVP_SERVER_BASE_URL")

        self.build_mgr = ArtifactoryBuildManager(
            self.artifactory_url, project="", auth=(user, password)
        )

    def _getenv(self, key):
        env_var = os.getenv(key)
        if not env_var:
            raise KeyError(f"Environment variable {key} is not set")

        return env_var

    def _get_artifactory_build_url(self, build_name, build_id, timestamp):
        return self.artifactory_url.replace(
            "/artifactory", f"/ui/builds/{build_name}/{build_id}/{timestamp}"
        )

    def get_kronos_fvp_builds(self):
        builds = self.build_mgr.get_build_runs(
            self._getenv("FVP_SERVER_FVP_BUILD_PATH")
        )
        df = (
            pd.DataFrame.from_dict(b.info["buildInfo"] for b in builds)
            .filter(items=["number", "started"])
            .rename(columns={"number": "fvp_pv"})
        )

        fvp_url = self._getenv("FVP_SERVER_FVP_BUILD_PATH").replace(
            "/", "%2F"
        )
        df["timestamp"] = pd.to_datetime(df["started"]).apply(
            lambda x: int(x.timestamp() * 1000)
        )
        df["fvp_build_url"] = df.apply(
            lambda x: self._get_artifactory_build_url(
                fvp_url, x["fvp_pv"], x["timestamp"]
            ),
            axis=1,
        )
        df = df.filter(items=["fvp_pv", "fvp_build_url"])

        return df

    def _is_pass(self, build_properties):
        api_url = build_properties["buildInfo.env.CI_API_V4_URL"]
        project_id = build_properties["buildInfo.env.CI_PROJECT_ID"]
        pipeline_id = build_properties["buildInfo.env.CI_PIPELINE_ID"]

        # Get is necessary here since Kronos golden version builds
        # won't have either PARENT_PROJECT_ID or PARENT_PIPELINE_ID
        parent_project_id = build_properties.get(
                "buildInfo.env.PARENT_PROJECT_ID")
        parent_pipeline_id = build_properties.get(
                "buildInfo.env.PARENT_PIPELINE_ID")

        # The GitLab response is paginated so iterate over the return
        # exhaustively.
        url = (
            f"{api_url}/projects/{project_id}/"
            f"pipelines/{pipeline_id}/jobs?"
            "pagination=keyset&per_page=20&order_by=id&sort=asc"
        )
        response = requests.head(url)
        if response.status_code != requests.codes.ok:
            return "Unknown"

        pages = int(response.headers["X-Total-Pages"])
        for page in range(1, pages+1):
            jobs = requests.get(f"{url}&page={page}").json()
            if any(
                job["stage"] == "Build" and job["status"] != "success"
                for job in jobs
            ):
                return "Fail"

        # If there isn't a parent pipeline then only Kronos build is used to
        # infer success. If there is, check the meta-arm pipeline, then
        # success = kronos success and meta-arm success.
        # Legacy builds which have a parent pipeline ID but not a parent
        # project ID will ignore the meta-arm success but these will be cleared
        # because of the retention policy
        if parent_pipeline_id and parent_project_id:
            bridges_url = (
                f"{api_url}/projects/{parent_project_id}/"
                f"pipelines/{parent_pipeline_id}/bridges"
            )
            response = requests.get(bridges_url)
            if response.status_code != requests.codes.ok:
                return "Unknown"

            bridges = response.json()
            for bridge in (
                    b for b in bridges if b["name"] == "trigger-meta-arm"):
                status = bridge["status"]
                stage = bridge["stage"]
                if status == "failed":
                    return "Fail"
                # Check if meta-arm is still running
                elif status != "success":
                    return "Pending"

        return "Pass"

    def get_kronos_image_builds(self):
        builds = self.build_mgr.get_build_runs(
            self._getenv("FVP_SERVER_IMAGE_BUILD_PATH")
        )
        df = (
            pd.DataFrame.from_dict(b.info["buildInfo"] for b in builds)
            .filter(
                items=[
                    "number",
                    "started",
                    "modules",
                    "properties",
                ]
            )
            .rename(columns={"number": "build_id"})
        )
        df["commit_sha"] = df.apply(
            lambda x: x["properties"]["buildInfo.env.CI_COMMIT_SHA"], axis=1
        )
        df["commit_slug"] = df.apply(
            lambda x: x["properties"]["buildInfo.env.CI_COMMIT_REF_SLUG"],
            axis=1,
        )
        # FVP might not be set if the build encounters an unexpected failure.
        df["fvp_pv"] = df.apply(
            lambda x: x["properties"].get("buildInfo.env.FVP_PV", ""), axis=1
        ).astype(str)
        df["pipeline_url"] = df.apply(
            lambda x: x["properties"]["buildInfo.env.CI_PIPELINE_URL"], axis=1
        )
        df["project_url"] = df.apply(
            lambda x: x["properties"]["buildInfo.env.CI_PROJECT_URL"], axis=1
        )
        df["override"] = df.apply(
            lambda x: "buildInfo.env.FVP_BUILD_NUMBER" in x["properties"],
            axis=1,
        )
        df["datetime"] = pd.to_datetime(df["started"]).dt.strftime(
            "%Y-%m-%d %H:%M"
        )

        # Only builds for "passed" pipelines will have artifacts
        df["pass"] = df.apply(
            lambda x: self._is_pass(x["properties"]), axis=1,)

        image_url = self._getenv("FVP_SERVER_IMAGE_BUILD_PATH").replace(
            "/", "%2F"
        )

        df["timestamp"] = pd.to_datetime(df["started"]).apply(
            lambda x: int(x.timestamp() * 1000)
        )
        df["image_build_url"] = df.apply(
            lambda x: self._get_artifactory_build_url(
                image_url, x["build_id"], x["timestamp"]
            ),
            axis=1,
        )

        df["artifact"] = df.apply(
            lambda x: ""
            if pd.isna(x["modules"])
            else x["modules"][0]["artifacts"][0]["path"],
            axis=1,
        )

        df = df.filter(
            items=[
                "build_id",
                "image_build_url",
                "datetime",
                "pass",
                "commit_sha",
                "commit_slug",
                "fvp_pv",
                "pipeline_url",
                "project_url",
                "override",
            ]
        )

        return df


class FVPData(object):
    """
    The context is of the format:
    {
      "last_pass": <image build id>,
      "last_fail": <image build id>,
      "data": {
        "<image build id>": {
          "build_id": <pipeline id>,
          "fvp_pv": <fvp pv>,
          "fvp_build_url": <fpv build url>,
          "override": [true|false],
          "datetime": <datetime>,
          "pass": [true|false],
          "commit_sha": <commit sha>,
          "commit_slug": <commit slug>,
          "pipeline_url": <pipeline url>,
          "project_url": <project url>,
          "image_build_url": <image build url>,
        }
      }
    }

    Image build ID above is the pipeline ID in the Kronos project
    """

    def __init__(self):
        self.artifactory_handler = ArtifactoryHandler()

        images_df = self.artifactory_handler.get_kronos_image_builds()
        fvp_df = self.artifactory_handler.get_kronos_fvp_builds()
        builds_df = fvp_df.merge(
            images_df, how="right", left_on="fvp_pv", right_on="fvp_pv"
        )
        builds_df = builds_df.sort_values(by=["datetime"], ascending=False)

        self._context = {}
        self._context["data"] = builds_df.to_dict("index")
        self._context["last_pass"] = next(
            (b for b, data in self._context["data"].items() if data["pass"]),
            None,
        )
        self._context["last_fail"] = next(
            (
                b
                for b, data in self._context["data"].items()
                if not data["pass"]
            ),
            None,
        )
        self._context["timestamp"] = dt.datetime.now()

    def get_context(self):
        return self._context


def get_template(name):
    template_dir = os.path.dirname(os.path.abspath(__file__))
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template_dir),
        extensions=["jinja2.ext.i18n"],
        autoescape=jinja2.select_autoescape(),
        trim_blocks=True,
        lstrip_blocks=True,
    )

    return env.get_template(name)


def render(context, output: pathlib.Path):
    if output.exists() and not output.is_dir():
        print(f"{output} is not a directory", file=sys.stderr)
        sys.exit(1)

    if not output.exists():
        output.mkdir(parents=True)

    with open(os.path.join(output, "index.html"), "wt") as f:
        f.write(get_template(f"report-index.html.jinja").render(context))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="fvp-summary")
    parser.add_argument("-o", "--output", type=pathlib.Path, required=True)
    args = parser.parse_args()

    fvp_data = FVPData()

    render(context=fvp_data.get_context(), output=args.output)
