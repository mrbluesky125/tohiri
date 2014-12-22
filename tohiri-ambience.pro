TEMPLATE = aux

NAME=harbour-tohiri-ambience

OTHER_FILES = \
        harbour-tohiri.ambience \
        images/* \
        rpm/*

ambience.files = \
        harbour-tohiri.ambience
        
ambience.path = /usr/share/ambience/$${NAME}

images.files = images/*
images.path = $${ambience.path}/images

INSTALLS += \
        ambience \
        images

