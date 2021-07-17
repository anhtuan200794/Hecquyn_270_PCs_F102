"""Synthesizes speech from the input string of text or ssml.

Note: ssml must be well-formed according to:
    https://www.w3.org/TR/speech-synthesis/
"""

import os 
from google.cloud import storage
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = 'D:/WorkSpace/01.Code/01.Sport1_Projects/01.Hecquyn_270_PCs_F102/PythonTextToSpeech/ggkey/mykey.json'

from google.cloud import texttospeech

# Instantiates a client
client = texttospeech.TextToSpeechClient()

# Set the text input to be synthesized
synthesis_input = texttospeech.SynthesisInput(text= "Bạn đang luyện tập với bài tập mặc định, chú ý giữ an toàn khi máy thay đổi độ dốc và tốc độ nhé")

# Build the voice request, select the language code ("en-US") and the ssml
# voice gender ("neutral")
voice = texttospeech.VoiceSelectionParams(
    language_code="vi-VN", ssml_gender=texttospeech.SsmlVoiceGender.MALE
)

# Select the type of audio file you want returned
audio_config = texttospeech.AudioConfig(
    audio_encoding=texttospeech.AudioEncoding.MP3, speaking_rate=0.9
)

# Perform the text-to-speech request on the text input with the selected
# voice parameters and audio file type
response = client.synthesize_speech(
    input=synthesis_input, voice=voice, audio_config=audio_config
)

# The response's audio_content is binary.
with open("output.mp3", "wb") as out:
    # Write the response to the output file.
    out.write(response.audio_content)
    print('Audio content written to file "output.mp3"')