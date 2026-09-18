set(ProjAI_Snake_Alsayek_Karabegovic_Snagic__NAME ProjAI_Snake_Alsayek_Karabegovic_Snagic_)				#Naziv prvog projekta u solution-u

file(GLOB ProjAI_Snake_Alsayek_Karabegovic_Snagic__SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB ProjAI_Snake_Alsayek_Karabegovic_Snagic__INCS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB ProjAI_Snake_Alsayek_Karabegovic_Snagic__INC_TD  ${NATID_SDK_INC}/td/*.h)
file(GLOB ProjAI_Snake_Alsayek_Karabegovic_Snagic__INC_GUI ${NATID_SDK_INC}/gui/*.h)
#Application icon
set(ProjAI_Snake_Alsayek_Karabegovic_Snagic__PLIST  ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/AppIcon.plist)
if(WIN32)
	set(ProjAI_Snake_Alsayek_Karabegovic_Snagic__WINAPP_ICON ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/winAppIcon.rc)
else()
	set(ProjAI_Snake_Alsayek_Karabegovic_Snagic__WINAPP_ICON ${CMAKE_CURRENT_LIST_DIR}/res/appIcon/winAppIcon.cpp)
endif()

# add executable
add_executable(${ProjAI_Snake_Alsayek_Karabegovic_Snagic__NAME} ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__INCS} ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__SOURCES} ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__INC_TD} ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__INC_GUI} ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__WINAPP_ICON})

source_group("inc"            FILES ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__INCS})
source_group("inc\\td"            FILES ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__INC_TD})
source_group("inc\\gui"            FILES ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__INC_GUI})
source_group("src"            FILES ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__SOURCES})

target_link_libraries(${ProjAI_Snake_Alsayek_Karabegovic_Snagic__NAME} debug ${MU_LIB_DEBUG} debug ${NATGUI_LIB_DEBUG} 
										optimized ${MU_LIB_RELEASE} optimized ${NATGUI_LIB_RELEASE})

setTargetPropertiesForGUIApp(${ProjAI_Snake_Alsayek_Karabegovic_Snagic__NAME} ${ProjAI_Snake_Alsayek_Karabegovic_Snagic__PLIST})

setAppIcon(${ProjAI_Snake_Alsayek_Karabegovic_Snagic__NAME} ${CMAKE_CURRENT_LIST_DIR})

setIDEPropertiesForGUIExecutable(${ProjAI_Snake_Alsayek_Karabegovic_Snagic__NAME} ${CMAKE_CURRENT_LIST_DIR})

setPlatformDLLPath(${ProjAI_Snake_Alsayek_Karabegovic_Snagic__NAME})
