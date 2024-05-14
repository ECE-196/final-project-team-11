from pydub import AudioSegment
import os


def split_audio(file_path, chunk_length_ms=5000, output_folder='output_chunks'):
    # Ensure the output folder exists
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # Load the audio file
    audio = AudioSegment.from_file(file_path)

    # Calculate the number of chunks
    duration_in_ms = len(audio)
    number_of_chunks = (duration_in_ms // chunk_length_ms) + \
        (1 if duration_in_ms % chunk_length_ms != 0 else 0)

    # Split the audio and save chunks
    for i in range(number_of_chunks):
        start_ms = i * chunk_length_ms
        end_ms = start_ms + chunk_length_ms
        chunk = audio[start_ms:end_ms]
        chunk.export(os.path.join(
            output_folder, f'chunk{i + 1}.mp3'), format="mp3")
        print(f"Exported chunk {i + 1}")


# Usage
split_audio('backend/data/bio.mp3')
