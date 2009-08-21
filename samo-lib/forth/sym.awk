# parse the following:
# : word :: assembler-name
# \ word :: assembler-name
# code word :: assembler-name
# also tries to handle : constant variable create
# and exclude them from inside definitions

BEGIN {
        _ord_init()
        delete done
        compiling = 0

        print "\\ generated file - do not modify"
        print
        print "only forth"
        print "also meta-words definitions"
        print "meta-compiler"
        print
}

# from the gnu/gawk manual
function _ord_init(    low, high, i, t)
{
        low = sprintf("%c", 7) # BEL is ascii 7
        if (low == "\a") {    # regular ascii
                low = 0
                high = 127
        } else if (sprintf("%c", 128 + 7) == "\a") {
                # ascii, mark parity
                low = 128
                high = 255
        } else {        # ebcdic(!)
                low = 0
                high = 255
        }

        for (i = low; i <= high; i++) {
                t = sprintf("%c", i)
                _ord_[t] = i
        }
}

function ord(str,    c)
{
        # only first character is of interest
        c = substr(str, 1, 1)
        return _ord_[c]
}

function chr(c)
{
        # force c to be numeric by adding 0
        return sprintf("%c", c + 0)
}

function asm_name(word, name) {
        if ("" == name) {
                for (i = 1; i <= length(word); ++i) {
                        c = tolower(substr(word, i, 1))
                        if ( "-" == c) {
                                c = "_"
                        } else if (c !~ /[[:alnum:]]/) {
                                c = sprintf("_%02x", ord(c))
                        }
                        name = name c
                }
        } else {
                name = tolower(name)
                gsub("-", "_", name)
        }
        return name
}


/[[:space:]]::[[:space:]]/ {
        word = tolower($2)
        name = tolower($4)
        if (!done[word]) {
                print ": " word "   output-symbol\" " asm_name(word, name) "\" ;"
                done[word] = 1
        }
}

/;[[:space:]]+immediate/ {
  print "  immediate"
}

# also detects start of compiled definition
# to avoid printing entries for defining words used
# inside colon definitions
/^[[:space:]]*(:|create|variable)[[:space:]]/ {
        def = tolower($1)
        word = tolower($2)
        if (!done[word] && !compiling) {
                print ": " word "   output-symbol\" " asm_name(word, "") "\" ;"
                done[word] = 1
        }
        if (":" == def) {
                compiling = 1
        }
}

/^([[:space:]]*|.*[[:space:]])constant[[:space:]]/ {
        word = gensub(/^(.*[[:space:]]|[[:space:]]*)constant[[:space:]]+([^[:space:]]+)([[:space:]]*|[[:space:]].*)$/, "\\2", 1)
        word = tolower(word)
        if (!done[word] && !compiling) {
                print ": " word "   output-symbol\" " asm_name(word, "") "\" ;"
                done[word] = 1
        }
}

# this also detects assemble comments
# hopefully this does not matter
/^([[:space:]]*|.*[[:space:]]);([[:space:]]*|[[:space:]].*)$/ {
        compiling = 0
}

