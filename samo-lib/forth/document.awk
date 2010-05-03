# parse the following:
# : word :: assembler-name ( stack )
# code word :: assembler-name ( stack )
# \ ** documentation or other text

BEGIN {
        print "Forth Words"
        print "==========="
        print
        suppress = 0
}

END {
        print
}

/^\\[[:space:]]+\*\*[[:space:]]/ {
        if (!suppress) {
                text = $0
                sub("^[[:space:]]*\\\\[[:space:]]*\\*\\*[[:space:]]", "", text)
                printf("    %s\n", text)
        }
}

/[[:space:]]::[[:space:]]/ {
        if ($1 == "\\") {
                suppress = 1
        } else {
                suppress = 0
                stack = $0
                type = $1
                word = $2
                if (type == ":") {
                        type = "colon"
                }
                sub("^.*::[^(]+", "", stack)
                printf("\n%-28s %-60s   [%s]\n", word, stack, type)
        }
}
