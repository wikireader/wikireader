# generat an application header

BEGIN {
        NAME_LENGTH = 32
        for (i = 0; i < NAME_LENGTH; ++i) {
                padding = padding "\0"
        }
        name = substr("No Name" padding, 0, NAME_LENGTH)
}

END {
        printf "SAMO" name
}

/^[[:space:]]*#[[:space:]]*define[[:space:]]+APPLICATION_TITLE[[:space:]]+/ {
        line = gensub("^.*APPLICATION_TITLE[[:space:]]*\"", "", 1, $0)
        line = gensub("\".*$", "", 1, line)
        if ("" != line) {
                name = substr(line padding, 0, NAME_LENGTH)
        }
}
