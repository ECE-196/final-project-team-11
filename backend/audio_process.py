import librosa

import numpy as np

import matplotlib.pyplot as plt


def process_audio(mp3_path):
    # Load the MP3 file
    # Load with the original sample rate
    y, sr = librosa.load(mp3_path, sr=None)

    # Calculate the length of each segment in samples
    segment_length = 5 * sr  # 5 seconds multiplied by the number of samples per second

    # Initialize a list to store the spectrogram matrices
    spectrograms = []

    # Process each 5-second segment
    for start in range(0, len(y), segment_length):
        end = start + segment_length
        # Ensure we don't exceed the length of the audio
        if end > len(y):
            end = len(y)
        # Extract the segment
        segment = y[start:end]

        # Generate the spectrogram for the segment
        # Here we're using a short-time Fourier transform (stft)
        S = np.abs(librosa.stft(segment))

        # Convert the complex values to decibel units
        S_dB = librosa.amplitude_to_db(S, ref=np.max)

        # Append the matrix to the list
        spectrograms.append(S_dB)

    first_spectrogram = spectrograms[0]

    # Plotting the first spectrogram
    plt.figure(figsize=(10, 4))
    librosa.display.specshow(first_spectrogram, sr=sr,
                             x_axis='time', y_axis='hz', hop_length=512)
    plt.colorbar(format='%+2.0f dB')
    plt.title('Spectrogram (dB)')
    plt.show()

    return spectrograms


# Path to your MP3 file
mp3_path = 'data/bio.mp3'

# Call the function
spectrogram_list = process_audio(mp3_path)

# Example: Print the shape of the first spectrogram matrix
print(spectrogram_list[0].shape)
