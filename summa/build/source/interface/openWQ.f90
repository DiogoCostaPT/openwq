module openwq

 USE, intrinsic :: iso_c_binding
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

   integer function openWQ_run_time_start(this, year, month, day, hour, minute)
      implicit none
      class(ClassWQ_OpenWQ) :: this
      integer, intent(in)   :: year
      integer, intent(in)   :: month
      integer, intent(in)   :: day
      integer, intent(in)   :: hour
      integer, intent(in)   :: minute
      openWQ_run_time_start = openwq_run_time_start_c(this%ptr, year, month, day, hour, minute)
   end function
   ! The func variables denotes which c++ function to call
   ! This is needed b/c we cannot create interface functions in
   ! Fortran that take the same variables
   ! func = 1: OpenWQ::run_time_start
   ! func = 2: OpenWQ::run_space
   ! func = 3: OpenWQ::run_time_end
   integer function openWQ_run(this, func)
      implicit none
      class(ClassWQ_OpenWQ) :: this
      integer, intent(in)   :: func
      openWQ_run = openwq_run_c(this%ptr, func)
   end function



end module openwq