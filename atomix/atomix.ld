SECTIONS
{
    .mod_init :
    {
        PROVIDE(__start_mod_init = .);
        KEEP(*(.mod_init))
        PROVIDE(__stop_mod_init = .);
    }
}