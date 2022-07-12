module summa_openWQ
  USE nrtype
  USE openWQ, only:ClassWQ_OpenWQ
  USE data_types, only:gru_hru_doubleVec
  implicit none
  private
  ! Subroutines
  public :: run_time_start
  public :: run_space
  public :: run_time_end

  ! Global Data for prognostic Variables of HRUs
  type(gru_hru_doubleVec),save,public   :: progStruct_timestep_start ! copy of progStruct at the start of timestep for passing fluxes


  contains

  ! Subroutine that SUMMA calls to pass varialbes that need to go to
  ! openWQ
subroutine run_time_start(openWQ_obj, summa1_struc)
  USE summa_type, only:summa1_type_dec            ! master summa data type
  USE globalData, only:gru_struc
  USE var_lookup, only: iLookPROG  ! named variables for state variables
  USE var_lookup, only: iLookTIME  ! named variables for time data structure

  implicit none

  ! Dummy Varialbes
  class(ClassWQ_OpenWQ), intent(in)  :: openWQ_obj
  type(summa1_type_dec), intent(in)  :: summa1_struc
  ! local variables
  integer(i4b)                       :: iGRU
  integer(i4b)                       :: iHRU
  integer(i4b)                       :: iVar
  integer(i4b)                       :: iDat
  integer(i4b)                       :: openWQArrayIndex
  integer(i4b)                       :: simtime(5) ! 5 time values yy-mm-dd-hh-min
  real(rkind)                        :: soilMoisture(sum(gru_struc(:)%hruCount))
  real(rkind)                        :: soilTemp(sum(gru_struc(:)%hruCount))
  real(rkind)                        :: airTemp(sum(gru_struc(:)%hruCount))
  real(rkind)                        :: swe_vol(sum(gru_struc(:)%hruCount))
  real(rkind)                        :: canopyWat_vol(sum(gru_struc(:)%hruCount))
  real(rkind)                        :: matricHead_vol(sum(gru_struc(:)%hruCount))
  real(rkind)                        :: aquiferStorage_vol(sum(gru_struc(:)%hruCount))
  integer(i4b)                       :: err

  summaVars: associate(&
      progStruct     => summa1_struc%progStruct             , &
      timeStruct     => summa1_struc%timeStruct               &
  )

  ! Assemble the data to send to openWQ
  openWQArrayIndex = 1 ! index into the arrays that are being passed to openWQ
  do iGRU = 1, size(gru_struc(:))
      do iHRU = 1, gru_struc(iGRU)%hruCount
        
        soilMoisture(openWQArrayIndex) = 0     ! TODO: Find the value for this varaibles

        soilTemp(openWQArrayIndex) = sum(progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%mLayerTemp)%dat(:)) / &
            size(progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%mLayerTemp)%dat(:)) 
        
        airTemp(openWQArrayIndex) = progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%scalarCanairTemp)%dat(1)
        
        swe_vol(openWQArrayIndex) = progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%scalarSWE)%dat(1)

        canopyWat_vol(openWQArrayIndex) = progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%scalarCanopyWat)%dat(1)

        matricHead_vol(openWQArrayIndex) = sum(progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%mLayerMatricHead)%dat(:)) / &
            size(progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%mLayerMatricHead)%dat(:))
        
        aquiferStorage_vol(openWQArrayIndex) = progStruct%gru(iGRU)%hru(iHRU)%var(iLookPROG%scalarAquiferStorage)%dat(1)
        
        ! Copy the prog structure
        do iVar = 1, size(progStruct%gru(iGRU)%hru(iHRU)%var)
          do iDat = 1, size(progStruct%gru(iGRU)%hru(iHRU)%var(iVar)%dat)
            progStruct_timestep_start%gru(iGRU)%hru(iHRU)%var(iVar)%dat(iDat) = progStruct%gru(iGRU)%hru(iHRU)%var(iVar)%dat(iDat)
          end do
        end do

        openWQArrayIndex = openWQArrayIndex + 1
      end do
  end do

  ! add the time values to the array
  simtime(1) = timeStruct%var(iLookTIME%iyyy)  ! Year
  simtime(2) = timeStruct%var(iLookTIME%im)    ! month
  simtime(3) = timeStruct%var(iLookTIME%id)    ! hour
  simtime(4) = timeStruct%var(iLookTIME%ih)    ! day
  simtime(5) = timeStruct%var(iLookTIME%imin)  ! minute

  err=openWQ_obj%run_time_start(&
        sum(gru_struc(:)%hruCount),             & ! total HRUs
        simtime,                                &
        soilMoisture,                           &                    
        soilTemp,                               &
        airTemp,                                &
        swe_vol,                                &
        canopyWat_vol,                          &
        matricHead_vol,                         &
        aquiferStorage_vol)

  ! copy progStruct values to progStruct_timestep_start


  end associate summaVars
end subroutine


subroutine run_space(openwq_obj,ihru, timeVec, progStruct, fluxData)
  USE var_lookup, only: iLookPROG  ! named variables for state variables
  USE var_lookup, only: iLookTIME  ! named variables for time data structure
  USE var_lookup, only: iLookFLUX  ! named varaibles for flux data
  USE data_types, only: var_dlength
  implicit none
  class(ClassWQ_OpenWQ), intent(in)    :: openWQ_obj
  integer(i4b),          intent(in)    :: ihru
  integer(i4b),          intent(in)    :: timeVec(:)          ! int vector   -- model time data
  type(var_dlength),     intent(in)    :: progStruct            ! x%var(:)%dat -- model prognostic (state) variables
  type(var_dlength),     intent(in)    :: fluxData            ! x%var(:)%dat -- model fluxes

end subroutine run_space


subroutine run_time_end(openWQ_obj, summa1_struc)
  USE summa_type, only:summa1_type_dec            ! master summa data type
  
  USE var_lookup, only: iLookTIME  ! named variables for time data structure

  implicit none

  ! Dummy Varialbes
  class(ClassWQ_OpenWQ), intent(in)  :: openWQ_obj
  type(summa1_type_dec), intent(in)  :: summa1_struc

  ! Local Variables
  integer(i4b)                       :: simtime(5) ! 5 time values yy-mm-dd-hh-min
  integer(i4b)                       :: err ! error control

  summaVars: associate(&
      timeStruct     => summa1_struc%timeStruct       &       
  )

  simtime(1) = timeStruct%var(iLookTIME%iyyy)  ! Year
  simtime(2) = timeStruct%var(iLookTIME%im)    ! month
  simtime(3) = timeStruct%var(iLookTIME%id)    ! hour
  simtime(4) = timeStruct%var(iLookTIME%ih)    ! day
  simtime(5) = timeStruct%var(iLookTIME%imin)  ! minute

  err=openwq_obj%run_time_end(simtime)           ! minute

  end associate summaVars
end subroutine



end module summa_openWQ