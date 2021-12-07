module openwq

 USE, intrinsic :: iso_c_binding
 private
 public :: openWQ_obj
!  public :: InitObj
!  public :: Decl
!  public :: RunTimeStart
!  public :: RunSpace
!  public :: RunTimeEnd

 include "openWQInterface.f90"
!  ! private :: foo, foo_speaker

 type openWQ_obj
    private
    type(c_ptr) :: ptr ! pointer to openWQ class

 contains
    procedure :: get_num => openWQ_get_num
 end type

 interface openWQ_obj
    procedure create_openwq
 end interface

 contains
    function create_openwq(num)
        implicit none
        type(openWQ_obj) :: create_openwq
        integer, intent(in) :: num
        create_openwq%ptr = create_openwq_c(num)
    end function

    integer function openWQ_get_num(this)
        implicit none
        class(openWQ_obj) :: this
        openwq_get_num = openwq_get_num_c(this%ptr)
    end function
!  ! Globaly accessible variable

!  type(c_ptr), save, public :: openWQ_obj

    
!     subroutine Decl(num_hrus)
!        integer(c_int), intent(in) :: num_hrus
!        ! Might need this eventually
!        ! if (c_associated(openWQ_obj)
!        ! endif
!        call openWQ_decl(openWQ_obj, num_hrus)
   
!        return
!     end subroutine Decl
   
!     subroutine RunTimeStart()
!        call openWQ_run_time_start(openWQ_obj)
!        return
!     end subroutine
   
!     subroutine RunSpace()
   
!        call openWQ_run_space(openWQ_obj)
    
!     end subroutine RunSpace
   
!     subroutine RunTimeEnd()
   
!        call openWQ_run_time_end(openWQ_obj)
    
!     end subroutine RunTimeEnd


end module openwq