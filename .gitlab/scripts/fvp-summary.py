#! /usr/bin/env python3

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
        password = self._getenv("ARTIFACTORY_KEY")
        user = self._getenv("ARTIFACTORY_USER")
        self.artifactory_url = self._getenv("ARTIFACTORY_BASE_URL")

        self.build_mgr = ArtifactoryBuildManager(
            self.artifactory_url, project="", auth=(user, password)
        )

    def _getenv(self, key):
        env_var = os.getenv(key)
        if not env_var:
            raise KeyError(f"Environment variable {key} is not set")

        return env_var

    def get_kronos_fvp_builds(self):
        builds = self.build_mgr.get_build_runs(
            self._getenv("ARTIFACTORY_FVP_BUILD_PATH")
        )
        df = pd.DataFrame.from_dict(
            b.info["buildInfo"] for b in builds
        ).filter(items=["number"])
        df["fvp_pv"] = df.apply(lambda x: f'0.0.{x["number"]}', axis=1)

        fvp_url = (
            self._getenv("ARTIFACTORY_FVP_BUILD_PATH")
            .replace("/", "%2F")
        )
        df["fvp_build_url"] = df.apply(
            lambda x: self.artifactory_url.replace(
                "/artifactory",
                f'/ui/builds/{fvp_url}/{x["number"]}',
            ),
            axis=1,
        )
        df = df.filter(items=["fvp_pv", "fvp_build_url"])

        return df

    def _is_pass(self, api_url, project_id, pipeline_id):
        r = requests.get(
            f"{api_url}/projects/{project_id}/pipelines/{pipeline_id}/jobs"
        )
        json = r.json()

        return all(
            job["status"] == "success" or job["stage"] != "Build"
            for job in json
        )

    def get_kronos_image_builds(self):
        builds = self.build_mgr.get_build_runs(
            self._getenv("ARTIFACTORY_IMAGE_BUILD_PATH")
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
            axis=1
        )
        df["fvp_pv"] = df.apply(
            lambda x: x["properties"]["buildInfo.env.FVP_PV"], axis=1
        ).astype(str)
        df["pipeline_url"] = df.apply(
            lambda x: x["properties"]["buildInfo.env.CI_PIPELINE_URL"], axis=1
        )
        df["project_url"] = df.apply(
            lambda x: x["properties"]["buildInfo.env.CI_PROJECT_URL"], axis=1
        )
        df["override"] = df.apply(
            lambda x: "buildInfo.env.FVP_BUILD_NUMBER" in x["properties"],
            axis=1
        )
        df["datetime"] = pd.to_datetime(df["started"].astype(str)).dt.strftime(
            "%Y-%m-%d %H:%M"
        )
        # Only builds for "passed" pipelines will have artifacts
        df["pass"] = df.apply(
            lambda x: self._is_pass(
                x["properties"]["buildInfo.env.CI_API_V4_URL"],
                x["properties"]["buildInfo.env.CI_PROJECT_ID"],
                x["properties"]["buildInfo.env.CI_PIPELINE_ID"],
            ),
            axis=1,
        )

        image_url = (
            self._getenv("ARTIFACTORY_IMAGE_BUILD_PATH")
            .replace("/", "%2F")
        )
        df["image_build_url"] = df.apply(
            lambda x: self.artifactory_url.replace(
                "/artifactory",
                f'/ui/builds/{image_url}/{x["build_id"]}',
            ),
            axis=1,
        )

        df["artifact"] = df.apply(
            lambda x: x["modules"][0]["artifacts"][0]["path"], axis=1
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
                "override"
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
            images_df, how="inner", left_on="fvp_pv", right_on="fvp_pv"
        )
        builds_df = builds_df.sort_values(by=["datetime"], ascending=False)
        builds_df = builds_df.set_index("build_id")

        self._context = {}
        self._context["data"] = builds_df.to_dict("index")
        self._context["last_pass"] = next(
            (b for b, data in self._context["data"].items() if data["pass"]),
            None
        )
        self._context["last_fail"] = next(
            (
                b for b, data in self._context["data"].items()
                if not data["pass"]
            ),
            None
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
