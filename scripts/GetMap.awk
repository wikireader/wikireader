# display the name of a mapfile from a config.h
# detect the appropriate:
#   #define BOARD_XXXXX 1
# and generates XXX.mapfile

BEGIN {
        mapfile = "wikireader.map"
}
END {
        print mapfile
}
/^[[:space:]]*#[[:space:]]*define[[:space:]]+BOARD_[^[:space:]]+[[:space:]]+1/ {
        line = gensub("^.*BOARD_", "", 1, $0)
        line = gensub("[[:space:]]+1[[:space:]]*$", "", 1, line)
        if ("" != line) {
                mapfile = line ".mapfile"
        }
}
