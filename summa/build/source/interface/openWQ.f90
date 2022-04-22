module openwq
   
 USE, intrinsic :: iso_c_binding
 USE nrtype
 private
 public :: ClassWQ_OpenWQ

 include "openWQInterface.f90"

 type ClassWQ_OpenWQ
    private
    type(c_ptr) :: ptr ! pointer to openWQ class

 contains
   !  procedure :: get_num => openWQ_get_num
    procedure :: decl => openWQ_init
    procedure :: run_time_start => openWQ_run_time_start
    procedure :: run => openWQ_run

 end type

 interface ClassWQ_OpenWQ
    procedure create_openwq
 end interface
 contains
    function create_openwq(num)
        implicit none
        type(ClassWQ_OpenWQ) :: create_openwq
        integer, intent(in) :: num
        create_openwq%ptr = create_openwq_c(num)
    end function

    ! supposed to be decl but needed to openWQ_decl in the interface file
    ! returns integer of either a failure(-1) or success(0)
   integer function openWQ_init(this)
      implicit none
      class(ClassWQ_OpenWQ) :: this
      openWQ_init = openwq_decl_c(this%ptr)
    end function
!  ! Globaly accessible variable

   integer function openWQ_run_time_start(this, numHRU, year, month, day, hour, minute, &
      soilMoisture, soilTemp, airTemp, swe_vol, canopyWat_vol, matricHead_vol, aquiferStorage_vol)
      implicit none
      class(ClassWQ_OpenWQ)      :: this
      integer(i4b), intent(in)   :: numHRU
      integer(i4b), intent(in)   :: year
      integer(i4b), intent(in)   :: month
      integer(i4b), intent(in)   :: day
      integer(i4b), intent(in)   :: hour
      integer(i4b), intent(in)   :: minute
      real(rkind),  intent(in)   :: soilMoisture(numHRU)
      real(rkind),  intent(in)   :: soilTemp(numHRU)
      real(rkind),  intent(in)   :: airTemp(numHRU)
      real(rkind),  intent(in)   :: swe_vol(numHRU)
      real(rkind),  intent(in)   :: canopyWat_vol(numHRU)
      real(rkind),  intent(in)   :: matricHead_vol(numHRU)
      real(rkind),  intent(in)   :: aquiferStorage_vol(numHRU)
      openWQ_run_time_start = openwq_run_time_start_c(this%ptr, numHRU, year, month, day, hour, minute, &
         soilMoisture, soilTemp, airTemp, swe_vol, canopyWat_vol, matricHead_vol, aquiferStorage_vol)
   end function

   integer function openWQ_run(this, func)
      implicit none
      class(ClassWQ_OpenWQ) :: this
      integer, intent(in)   :: func
      openWQ_run = openwq_run_c(this%ptr, func)
   end function


   


end module openwq