# check spelling
# see https://cspell.org/docs/installation/
# and https://github.com/streetsidesoftware/vscode-spell-checker/blob/main/FAQ.md

# go to root top level directory
# install cpsell with yarn add --dev cspell@latest

# first time installation
#  1) run `npx cspell "**" --no-progress --unique > words-found-verbose.txt`
#  and check if there are folders that you should be added to the ignore list in cpsell.json
#
#  2) then to build up the dictionary with just the words
#  % npx cspell --words-only --unique "**" | sort --ignore-case > project-words.txt

# To run a full check on the code do:
#  % npx cspell "**"
