# parse the following:
# : word :: assembler-name
# \ word :: assembler-name
# code word :: assembler-name

/[[:space:]]::[[:space:]]/ {
        word = tolower($2)
        asm_name = tolower($4)
        if ("" == asm_name) {
                asm_name = word
        }
        gsub("-", "_", asm_name)
        print ": " word "   output-symbol\" " asm_name "\" ;"
}

/;[[:space:]]+immediate/ {
  print "  immediate"
}
