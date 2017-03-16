#!/bin/bash

THIS_DIR=$(dirname "$0")

GPL_LIC="$THIS_DIR/../doc/templates/header.gpl"
LGPL_LIC="$THIS_DIR/../doc/templates/header.lgpl"
LICFILE=

declare -i end_of_options=0
declare -i year=$(date +%Y)

BASENAME_F=
AUTHOR_LIST=
CREATION_DATE=

generateInfo()
{
    fname=$1

    #get the basename
    BASENAME_F=$(basename "$fname")

    #get the creation time
    local info_str=$(git log --pretty="format:%an|%ae|%ai|%s" "$fname" | tail -n1)

    if [ -z "$info_str" ]; then
        echo "Cannot git log $fname"
    fi

    oldIFS=$IFS
    IFS="|"
    INFOS=($info_str)
    IFS=$oldIFS

    CREATION_DATE=${INFOS[2]}

    #create list of all authors using git blame
    oldIFS=$IFS
    IFS="|"

    ALL_AUTHORS=$(git blame -f "$f" | perl -w -e '
my %authorToNumLines = ();
my @files = ();
while (<>) {
  if (/(\S+)\s+\(([^[:digit:]]*[^[:digit:][:space:]]).*\)/) {
    $file = $1;
    $author = $2;
    push @files, $file;
    # Fix
    if ($author eq "Kristian") {
      $author = "Kristian Sons";
    }
    if ($author eq "Sergiy Byelozoyorov") {
      $author = "Sergiy Byelozyorov";
    }
    if ($author eq "kdahm") {
      $author = "Ken Patrick Dahm";
    }
    if (defined $authorToNumLines{$author}) {
      $authorToNumLines{$author} = $authorToNumLines{$author} + 1;
    }
    else {
      $authorToNumLines{$author} = 1;
    }
  }
}

while (($key, $value) = each %authorToNumLines) {
  $numLinesToAuthor{$value} = $key;
}

my @sorted_keys = sort { ($a <=> $b)*-1 } keys %numLinesToAuthor;

foreach my $key ( @sorted_keys ) {
  $value = $numLinesToAuthor{$key};
  if ($value eq "unknown") {

  }
  else
  {
    print "$value, ";
  }
}')
    IFS=$oldIFS

    AUTHOR_LIST=$(echo ${ALL_AUTHORS} | sed -e 's/.$//')
    echo ${AUTHOR_LIST}
}

fixFile()
{

    if [ -z "$LICFILE" ]; then
        echo >&2 "No license specified"
        exit 1
    fi

    for f; do
        if grep Copyright "$f" >/dev/null; then
            echo >&2 "Copyright notice in file $f, don't do anything !!!!"
        fi

        generateInfo "$f"
        sed -e "s|<year>|$year|;\
        s|<name of author>|$author|;\
        s|<name of file>|${BASENAME_F}|;\
        s|<authors>|${AUTHOR_LIST}|;\
        s|<date of creation>|${CREATION_DATE}|;" "$LICFILE"  > "${f}.tmp" && \
        cat "$f" >>  "${f}.tmp" && \
        mv "${f}.tmp" "$f"
        echo "Modified $f"
    done
}

while [ $# -ne 0 ]; do
    case "$1" in
    --)
        end_of_options=1
        ;;
    --gpl|--GPL)
            LICFILE=$GPL_LIC
            ;;
    --lgpl|--LGPL)
            LICFILE=$LGPL_LIC
            ;;
    --year=*)
            year=${1:7}
        if [ -z "$year" ]; then
                echo >&2 "No year specified in option: $1 (e.g. --year=2010)"
            exit 1
        fi
        ;;
        --author=*)
            author=${1:9}
        if [ -z "$author" ]; then
                echo >&2 "No author specified in option: $1 (e.g. --author=Name)"
            exit 1
        fi
            ;;
    -*)
        echo >&2 "unknown option $1"
        exit 1
        ;;
    *)
            fixFile "$1"
            ;;
    esac
    shift
    [ $end_of_options -eq 1 ] && break
done
