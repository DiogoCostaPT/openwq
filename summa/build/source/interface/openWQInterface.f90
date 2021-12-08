interface test
    function create_openwq_c(num) bind(C, name="create_openwq")
        use iso_c_binding
        implicit none
        type(c_ptr) :: create_openwq_c
        integer(c_int), value :: num
    end function

    ! function openwq_get_num_c(openWQ) bind(C, name="openwq_getNum")
    !     use iso_c_binding
    !     implicit none
    !     integer(c_int) :: openwq_get_num_c
    !     type(c_ptr), intent(in), value :: openWQ
    ! end function

    function openwq_decl_c(openWQ) bind(C, name="openwq_decl")
        use iso_c_binding
        implicit none
        integer(c_int) :: openwq_decl_c ! returns a return value of 0 (success) or -1 (failure)
        type(c_ptr), intent(in), value :: openWQ
    end function

end interface test