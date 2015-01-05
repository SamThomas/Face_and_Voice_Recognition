#/bin/bash

#arecord -f dat -d 2 sample.wav sysdefault:CARD=0
echo 'Ouverture du bluetooth'
obexpushd -B

echo 'Attente de message '
bcontinue=0

while [ $bcontinue == 0 ]
do
	if [ -f "sample1.wav" ]; then
	echo 'Fichier trouve'
	bcontinue=1
	else
	echo''
	fi
done 

echo 'Fin de l enregistrement' 
kill -12 $1
