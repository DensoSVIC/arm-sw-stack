# Copyright (c) 2023, Arm Limited.
#
# SPDX-License-Identifier: MIT

# A Sphinx extension to automatically generate a reference pointing to a path
# in the repository using the correct refspec.
#
# It can be used as follows:
#   :repo:`path/to/file.bb`
#
# It takes one configuration value, repo_url_pattern, which should be set in
# conf.py, either directly or using an environment variable. {ref} and {path}
# will be substituted at build-time, e.g.:
# repo_url_pattern = 'https://url.to.git.repo/tree/{ref}/{path}'


from docutils import nodes
import subprocess


def find_ref(config):
    # Obtain the version from the html_context
    if 'gitlab_version' in config.html_context:
        return config.html_context['gitlab_version']
    else:
        # Fall back to git
        output = subprocess.check_output(['git', 'rev-parse', 'HEAD'])
        return output.decode().strip()


def repo_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    config = inliner.document.settings.env.app.config
    pattern = config.repo_url_pattern
    if pattern:
        # Read the Docs sometimes adds extra quotes to environment variables
        pattern = pattern.strip("'")
        ref = find_ref(config)
        url = pattern.format(ref=ref, path=text)
        node = nodes.reference(rawtext, text, refuri=url, **options)
    else:
        # If the configuration value is not set, fall back to a literal
        node = nodes.literal(rawtext, text)
    return [node], []


def setup(app):
    app.add_role('repo', repo_role)
    app.add_config_value('repo_url_pattern', None, 'env')
