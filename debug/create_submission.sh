#!/usr/bin/env bash

# color variables
RED='\033[1;31m'    # red
YELLOW='\033[1;33m' # yellow
GREEN='\033[1;32m'  # green
BLUE='\033[0;34m'   # blue
RESET='\033[0m'     # no color

# Print a warning for the user.
#
# @param First argument is the warning message to print to the user.
#
print_warning() {
  echo -n -e "$YELLOW"
  echo -e "[WARNING] $1"
  echo -n -e "$RESET"
}

# Print an error message for the user
#
# @param First argument is the error message to print to the user.
#
print_error() {
  echo -n -e "$RED"
  echo -e "[ERROR] $1"
  echo -n -e "$RESET"
}

# Print a log for the user.
#
# @param First argument is the log message to print to the user.
#
print_log() {
  echo -n -e "$GREEN"
  echo -e "$1"
  echo -n -e "$RESET"
}

# Print a message for the user.
#
# @param First argument is the message to print to the user.
#
print_message() {
  echo -n -e "$BLUE"
  echo -e "$1"
  echo -n -e "$RESET"
}

# Print the usage description for this program, usually program exits after
# this.
#
print_usage() {
  echo -e "usage: $0 USERNAME"
  echo -e ''
  echo -e 'Create a submission from git diff files of your current repository.'
  echo -e ''
  echo -e 'options:'
  echo -e ''
  echo -e 'USERNAME         Your RHIT username to be appended to your submission.'
}

# Grab config variables
USERNAME=$1
LAB_BRANCH=main
TEMP_FILE=/tmp/$(printf '%s' "names.txt" | md5sum | cut -d' ' -f1).txt

# basic error checking
if [ -z "${USERNAME}" ]; then
    print_error "Please provide your username to append to your submission."
    echo -e ''
    print_usage
    exit 99
fi

# check if zip is installed
if ! command -v zip &> /dev/null
then
    print_error "You do no seem to have 'zip' installed."
    print_error "\t Try running 'sudo apt install -y zip' to install it!"
    print_error "\t If the error persists, please contact your instructor."
    exit 99
fi

# all good!
rm -f submission_${USERNAME}.zip
rm -f $TEMP_FILE || true

print_log "Process started: writing temporaries to $TEMP_FILE"

# find modified files
for i in `git status --short . | sed s/^...//`
do
    FNAME="$(find . -name $(basename $i) -print)"
    echo $FNAME >> $TEMP_FILE
done

# be verbose so students can double check.
print_log "Found the following modified files:"
cat $TEMP_FILE

# create the zip file
print_log "Creating the submission zip file."
cat $TEMP_FILE | zip submission_${USERNAME}.zip -@

# done, cleanup and print submission message
rm -f $TEMP_FILE
print_log "Done..."

print_message "################################################################"
print_message "        submission_${USERNAME}.zip has been created.            "
print_message "   Please submit THIS FILE AND THIS FILE ONLY to Gradescope.    "
print_message "################################################################"

