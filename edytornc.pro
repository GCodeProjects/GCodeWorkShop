SUBDIRS += src
TEMPLATE = subdirs

SUBDIRS = src sfs

src.file = src/src.pro
sfs.file = src/src_sfs.pro

CONFIG += warn_on \
          qt \
          thread
TRANSLATIONS = src/edytornc_pl.ts \
    src/edytornc_ca.ts \
    src/edytornc_de.ts \
    src/edytornc_fi.ts \
    src/edytornc_cs_CZ.ts \
    src/edytornc_es.ts \
    src/edytornc_nl.ts
