import librosa

import numpy as np

import matplotlib.pyplot as plt


def process_audio(mp3_path):

    y, sr = librosa.load(mp3_path, sr=None)

    segment_length = 5 * sr

    spectrograms = []

    for start in range(0, len(y), segment_length):
        end = start + segment_length
        if end > len(y):
            end = len(y)
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
mp3_path = 'backend/data/bio.mp3'

# Call the function
spectrogram_list = process_audio(mp3_path)

# Example: Print the shape of the first spectrogram matrix
print(spectrogram_list[0].shape)
