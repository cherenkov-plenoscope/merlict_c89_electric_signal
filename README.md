merlict_c89 single photon
=========================
[![Build Status](https://travis-ci.org/cherenkov-plenoscope/merlict_development_kit.svg?branch=master)](https://travis-ci.org/cherenkov-plenoscope/merlict_c89)

An add-on-library to ```merlict_c89```

A simple simulation of the signal-processing commonly used in the atmospheric Cherenkov-method in e.g. Cherenkov-telescopes. Convert photons to electric signals, extract arrival-times from the signals, and export. We abstract away analog time-series by using the concept of pulses. Pulses represent __single__-photons, and relevant artifacts of the read-out. A pulses only property is its arrival-time.

##Photo-electric-converter
We can simulate the basic properties of PhotoMultiplier-Tubes (PMT), and Silicon-PhotoMultipliers (SiPMs, or MPPCs, or arrays of G-APDs).

### photon-detection-efficieny vs. wavelength
Applicable for both PMTs and SiPMs. The probability to emitt an electric pulse given an incoming photon with a specific wavelength.

### Dark rate
The rate of pulses emitted by a photo-electric-converter when there is no light.
Almost only relevant for SiPMs. Commercial 9mm^2 SiPMs from e.g. Hamamatsu can have approx. 10e6s^{-1} accidental pulses at room-temperature. We inject accidental pulses with a Poisson-distribution between their arrival-time-intervals.

### cross-talk
The probability for emitting a second, additional pulse for a given first, initial pulse. Almost only relevant for SiPMs. Depending on electric potentials, SiPM are often operated to have below 10% probability to cross-talk. It is called cross-talk because the SiPM's Geiger-mode-avalanche-photo-diodes (G-APDs) can emitting infrared photons while emitting a pulse, and thus cross-talk into other G-APDs which then can emitt pulses themselves.

## Read-out
We assume that any readout can provide a list of the pulses arrival-times. From experience, we know that this is well posible down to the single-photons. We expect the readout to be as capable as the 2011 read-out, and pulse-reconstruction demonstrated in the [FACT-Cherenkov-telescope](https://pos.sissa.it/301/801/pdf).

### Arrival-time-spread
The stdandard-deviation of the reconstructed arrival-time of a pulse. For FACT this about 450ps for __single__-photons in the actual night-sky of La Palma. Not in your cosy and comfy lab where you shoot LAzZoRs with gazillions of photons. (Sorry for the technical jargon).

## Read and write single-photons -- Photon-Stream
We read and write the extracted pulses as a list of lists of arrival-times.
