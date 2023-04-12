# SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# This plugin is part of the Gitlab CI setup and is not made available with this
# git repository.
require 'embed-a-dangerfiles'

# Uncomment the following line to get debug output
# @verbose = true

Embed_A::Dangerfiles.for_project(self) do |dangerfiles|
  # Import all plugins from the gem
  dangerfiles.import_plugins

  # Import all rules except a subset of rules
  dangerfiles.import_dangerfiles(except: %w[changelog])
end

# Warn if the MR changes the Dangerfile
if git.modified_files.include? "Dangerfile"
  warn "This MR modifies Dangerfile! Watch for the rules!"
end

# Warn about remaining TODO's
todoist.warn_for_todos
todoist.print_todos_table
