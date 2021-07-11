"""Synthesizes speech from the input string of text or ssml.

Note: ssml must be well-formed according to:
    https://www.w3.org/TR/speech-synthesis/
"""
from google.cloud import texttospeech

def implicit():
    from google.cloud import storage

    # If you don't specify credentials when constructing the client, the
    # client library will look for credentials in the environment.
    storage_client = storage.Client()

    # Make an authenticated API request
    buckets = list(storage_client.list_buckets())
    print(buckets)
# Instantiates a client
client = texttospeech.TextToSpeechClient()

# Set the text input to be synthesized
synthesis_input = texttospeech.SynthesisInput(text= "Chào mừng bạn đến với máy tập thông minh Hec Quyn. Bạn hãy dành 30 giây để nghe huấn luyện viên Sport1 hướng dẫn cách sử dụng máy.Phím start để bắt đầu. Phím Stốp để dừng lại. Nhấn và giữ phím Stốp 3 giây để tắt hoặc bật chế độ huấn luyện viên. Phím Program để chọn 1 trong 12 bài tập mặc định sẵn, máy tự động tăng giảm tốc độ và độ dốc trong khoảng thời gian 30 phút, chú ý những bài tập này thường dùng cho người tập quen. Phím Mode để bạn lựa chọn cài đặt như: thời gian ,khoảng cách  ,calo tiêu hao. Trước khi bắt đầu chạy bạn nên đi giày và dành khoảng 5 phút để khởi động bằng các chức năng đã tích hợp trên máy như xoay eo,gập bụng. bạn hãy khởi động cùng tôi nhé! Xoay eo: bạn để bàn xoay xuống đất cạnh đầu máy tập , chân đứng chữ V trên bàn xoay, tay bám lên tay cầm của máy rồi xoay từ phải qua trái và ngược lại ,chú ý: khi xoay giữ nguyên vai chỉ chuyển động phần thân dưới. Xoay 15 đến 20 cái trong 1 hiệp, nên xoay từ 3 đến 5 hiệp. Gập bụng: 2 chân để vào giữa mút xốp của hai thanh thép ngang trên đầu máy, chân để trùng, lưng cong, ngả xuống hít sâu, gập lên thở ra. Gập 15 đến 20 cái trong 1 hiệp. Bạn nên  tập từ 3 đến 5 hiệp. Bây giờ bạn vào bài tập chạy cùng tôi nhé.")

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