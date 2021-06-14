"""Synthesizes speech from the input string of text or ssml.

Note: ssml must be well-formed according to:
    https://www.w3.org/TR/speech-synthesis/
"""
from google.cloud import texttospeech

# Instantiates a client
client = texttospeech.TextToSpeechClient()

# Set the text input to be synthesized
synthesis_input = texttospeech.SynthesisInput(text="Chào mừng bạn đến với máy tập thông minh Zeus để kết nối với cả thế giới. Khi bạn luyện tập có thể không cầm theo được điện thoại hay laptop, nhưng thần Zeus vẫn giúp bạn gọi điện, nhắn tin, chat zalo, viber hay check mail, lướt Facebook một cách dễ dàng. Bạn có thể chọn bỏ qua màn hình khai báo thông tin ban đầu bằng việc ấn vào mũi tên quay ngược ở dưới màn hình. Để thực hiện đồng bộ hóa điện thoại với máy tập, bạn mở điện thoại, bật chế độ Bluetooth để kết nối điện thoại với máy tập. Tiếp theo bạn ấn vào ô có chữ Applications để vào Zalo, Viber bằng cách check mã QR và đăng nhập Email, Facebook, Messenger. Để gọi điện bạn vào mục Sport one để ấn số gọi trực tiếp hoặc tìm tên theo danh bạ đã đồng bộ")

# Build the voice request, select the language code ("en-US") and the ssml
# voice gender ("neutral")
voice = texttospeech.VoiceSelectionParams(
    language_code="vi-VN", ssml_gender=texttospeech.SsmlVoiceGender.FEMALE
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