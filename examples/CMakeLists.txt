cmake_minimum_required (VERSION 2.8)
install (FILES all_default all_mono all_text basic_default basic_mono basic_text
         elapsed_default elapsed_mono elapsed_text estimate_default
         estimate_mono estimate_text label_default label_mono label_text
         minimal_default minimal_mono minimal_text percentage_default
         percentage_mono percentage_text remove_default remove_mono
         remove_text DESTINATION ${VRAMSTEG_DOCDIR}/examples)
