Name: Armando Partida Sanabia
Class: CPSC 4040

To encode a file:
./final-project -e <cover image name> <data file>

This operation will output a new image named "encoded-image.png"

To decode a file:
./final-project -d <encoded image>

Working examples:
(Hiding ASCII art file)
./final-project -e images/high-res.jpg text-files/hidden-message
./final-project -d encoded-image.png

(Hiding image)
./final-project -e cover-image.jpg images/droplets.jpg
./final-project -d encoded-image.png
rename decoded-file to decoded-file.jpg

(Hiding audio file)
./final-project -e cover-image.jpg audio/sample-3s.mp3
./final-project -d encoded-image.png
rename decoded-file to decoded-file.mp3

Tips:
-High res images will have more space for embedding data
-Compressing the encoded-image.png will break the embedded data
-File extensions must be manually added for the decoded file