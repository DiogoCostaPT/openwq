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

    function openwq_run_time_start_c(openWQ, year, month, hour, day, minute) bind(C, name="openwq_run_time_start")
        use iso_c_binding
        implicit none
        integer(c_int)                       :: openwq_run_time_start_c ! returns 0 (success) or -1 (failure)
        type(c_ptr), intent(in), value       :: openWQ
        integer(c_int), intent(in), value    :: year
        integer(c_int), intent(in), value    :: month
        integer(c_int), intent(in), value    :: hour
        integer(c_int), intent(in), value    :: day
        integer(c_int), intent(in), value    :: minute
    end function

    function openwq_start_time_hru_info_c(openWQ, soilMoisture, soilTemp, airTemp, & 
        swe_vol, canopyWat_vol, matricHead_vol, aquiferStorage_vol) &
        bind(C, name="openwq_start_time_hru_info")
        use iso_c_binding
        implicit none
        integer(c_int)                          :: openwq_start_time_hru_info_c
        type(c_ptr), intent(in), value          :: openWQ
        integer, intent(in), value              :: soilMoisture
        real(c_double), intent(in), value       :: soilTemp
        real(c_double), intent(in), value       :: airTemp
        real(c_double), intent(in), value       :: swe_vol
        real(c_double), intent(in), value       :: canopyWat_vol
        real(c_double), intent(in), value       :: matricHead_vol
        real(c_double), intent(in), value       :: aquiferStorage_vol
    end function

    function openwq_run_c(openWQ, func) bind(C, name="openwq_run")
        use iso_c_binding
        implicit none
        integer(c_int) :: openwq_run_c ! returns 0 (success) or -1 (failure)
        type(c_ptr), intent(in), value      :: openWQ
        integer(c_int), intent(in), value   :: func
    end function

end interface