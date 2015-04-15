SUBDIRS += src
TEMPLATE = subdirs 
CONFIG += warn_on \
          qt \
          thread 
TRANSLATIONS = src/edytornc_pl.ts \
    src/edytornc_ca.ts \
    src/edytornc_de.ts \
    src/edytornc_fi.ts \
    src/edytornc_cs_CZ.ts \
    src/edytornc_es.ts
