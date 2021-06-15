"""Synthesizes speech from the input string of text or ssml.

Note: ssml must be well-formed according to:
    https://www.w3.org/TR/speech-synthesis/
"""
from google.cloud import texttospeech

# Instantiates a client
client = texttospeech.TextToSpeechClient()

# Set the text input to be synthesized
synthesis_input = texttospeech.SynthesisInput(text= "Chúc mừng bạn đã hoàn thành buổi tập . Bạn không nên ngồi xuống ngay hãy đi lại nhẹ nhàng và hít thở đều. Bước 8, bạn sử dụng máy mát xa năm đến mười phút để thả lỏng cơ bằng cách khoác dây đai mát xa vào người và ấn công tắc mở để bắt đầu mát xa, di chuyển dây đai mát xa đến các vị trí như bụng ,lưng vai gáy , mông đùi, mỗi vị trí mát xa khoảng một phút rồi di chuyển dây đai đến vị trí khác. Bước 9. Khi tập luyện xong bạn nên uống nước lọc ,không lên uống nước có ga . Chú ý: khi uống nước theo ngụm nhỏ uống từ từ , không lên uống nhanh. Với 30 phút tập luyện mỗi ngày bạn sẽ có một dáng vóc hoàn hảo, đặc biệt tốt cho hệ tim mạch, tăng cường trao đổi chất, giúp cơ thể cải thiện hệ thống xương khớp, tăng sự dẻo dai cho cơ thể. Nếu bạn là một chuyên gia thường xuyên luyện tập có thể bỏ qua chức năng Huấn luyện viên để quan tâm đến các chức năng theo dõi tim mạch calo bằng cách nhấn nút Stop 3 giây khi khởi động máy để bỏ qua Trợ lý ảo nhé")

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