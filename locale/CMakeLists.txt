file (GLOB es_PO_FILES ${dir}/locale/lang/*)
set(es_POT ${dir}/locale/emulationstation2.pot)

# Compile the already-committed .po files into .mo files for runtime use.
# This never touches the tracked .pot/.po sources, so it's safe to run on every build.
add_custom_target (i18n ALL COMMENT "Compiling i18n messages.")

foreach (es_PO_INPUT ${es_PO_FILES})
        get_filename_component (es_PO_LANG ${es_PO_INPUT} NAME_WE)
	set (es_MO_OUTPUT ${es_PO_INPUT}/LC_MESSAGES/emulationstation2.mo)
        add_custom_command (TARGET i18n COMMAND ${MSGFMT_EXECUTABLE} -o ${es_MO_OUTPUT} ${es_PO_INPUT}/LC_MESSAGES/emulationstation2.po)
        install (FILES ${es_MO_OUTPUT} DESTINATION share/locale/${es_PO_LANG}/LC_MESSAGES RENAME emulationstation2.mo)
endforeach (es_PO_INPUT ${es_PO_FILES})

# Regenerate the .pot from source strings and merge it into every .po - this rewrites
# tracked translation files in place, so it's opt-in only. Run `make i18n-update`
# manually when actually refreshing translations; never part of the default build.
add_custom_target (i18n-update COMMENT "Updating i18n message catalogs from source.")
add_custom_command (TARGET i18n-update COMMAND find "${dir}/es-app" "${dir}/es-core" -name "*.cpp" -o -name "*.h" | ${XGETTEXT_EXECUTABLE} --add-comments=TRANSLATION -f - -o ${es_POT} --no-location --keyword=_)
add_custom_command (TARGET i18n-update COMMAND sed -i '/^\"POT-Creation-Date:/d' ${es_POT})
foreach (es_PO_INPUT ${es_PO_FILES})
	add_custom_command (TARGET i18n-update COMMAND ${MSGMERGE_EXECUTABLE} -U --no-fuzzy-matching ${es_PO_INPUT}/LC_MESSAGES/emulationstation2.po ${es_POT})
endforeach (es_PO_INPUT ${es_PO_FILES})

add_custom_command (TARGET i18n-update COMMAND echo)
foreach (es_PO_INPUT ${es_PO_FILES})
  add_custom_command (TARGET i18n-update COMMAND printf "%-5s " `basename ${es_PO_INPUT}` && LANG=C ${MSGFMT_EXECUTABLE} -o /dev/null ${es_PO_INPUT}/LC_MESSAGES/emulationstation2.po --statistics)
endforeach (es_PO_INPUT ${es_PO_FILES})
add_custom_command (TARGET i18n-update COMMAND echo)
