TEMPLATE = aux

NAME = harbour-ambience-tohiri

OTHER_FILES = $${NAME}.ambience images/$${NAME}.jpg

ambience.files = $${NAME}.ambience
ambience.path = /usr/share/ambience/$${NAME}

images.files = images/$${NAME}.jpg
images.path = $${ambience.path}/images

INSTALLS += ambience images

