interface
    function create_openwq_c(num) bind(C, name="create_openwq")
        use iso_c_binding
        implicit none
        type(c_ptr) :: create_openwq_c
        integer(c_int), value :: num
    end function

    function openwq_decl_c(openWQ) bind(C, name="openwq_decl")
        use iso_c_binding
        implicit none
        integer(c_int) :: openwq_decl_c ! returns a return value of 0 (success) or -1 (failure)
        type(c_ptr), intent(in), value :: openWQ
    end function

    function openwq_run_time_start_c(openWQ, numHRU, year, month, hour, day, minute, &
        soilMoisture, soilTemp, airTemp, swe_vol, canopyWat_vol, &
        matricHead_vol, aquiferStorage_vol) bind(C, name="openwq_run_time_start")
        use iso_c_binding
        implicit none
        integer(c_int)                       :: openwq_run_time_start_c ! returns 0 (success) or -1 (failure)
        type(c_ptr),    intent(in), value    :: openWQ
        integer(c_int), intent(in), value    :: numHRU
        integer(c_int), intent(in), value    :: year
        integer(c_int), intent(in), value    :: month
        integer(c_int), intent(in), value    :: hour
        integer(c_int), intent(in), value    :: day
        integer(c_int), intent(in), value    :: minute
        real(c_double), intent(in)           :: soilMoisture(numHRU)
        real(c_double), intent(in)           :: soilTemp(numHRU)
        real(c_double), intent(in)           :: airTemp(numHRU)
        real(c_double), intent(in)           :: swe_vol(numHRU)
        real(c_double), intent(in)           :: canopyWat_vol(numHRU)
        real(c_double), intent(in)           :: matricHead_vol(numHRU)
        real(c_double), intent(in)           :: aquiferStorage_vol(numHRU)
    end function

    function openwq_run_c(openWQ, func) bind(C, name="openwq_run")
        use iso_c_binding
        implicit none
        integer(c_int) :: openwq_run_c ! returns 0 (success) or -1 (failure)
        type(c_ptr), intent(in), value      :: openWQ
        integer(c_int), intent(in), value   :: func
    end function

end interface