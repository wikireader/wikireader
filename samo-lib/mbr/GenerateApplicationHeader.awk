# generate an application header

BEGIN {
        NAME_LENGTH = 32
        for (i = 0; i < NAME_LENGTH; ++i) {
                padding = padding "\0"
        }
        n = 0
        name[0] = substr("No Name" padding, 0, NAME_LENGTH)
}

END {
        printf("SAMO%c\0\0\0", n)
        for (i = 0; i < n; ++i) {
                printf(name[i])
        }
}

/^[[:space:]]*#[[:space:]]*define[[:space:]]+APPLICATION_TITLE[[:digit:]]*[[:space:]]+/ {
        line = gensub("^.*APPLICATION_TITLE[[:digit:]]*[[:space:]]*\"", "", 1, $0)
        line = gensub("\".*$", "", 1, line)
        if ("" != line) {
                name[n] = substr(line padding, 0, NAME_LENGTH)
                n = n + 1
        }
}
