.text
.globl incrementClicks

incrementClicks:
    lock incl (%rcx)
    ret