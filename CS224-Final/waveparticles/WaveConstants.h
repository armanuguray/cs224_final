#ifndef WAVECONSTANTS_H
#define WAVECONSTANTS_H

// The speed of each wave particle, in world coordinates per second
#define WAVE_SPEED 100

// The minimum amplitude of each particle, in world coordinates (distance from projection grid plane).
// When a particle's amplitude is less than WAVE_MIN_AMPLITUDE, it dies
#define WAVE_MIN_AMPLITUDE .1

// The rate at which the amplitude of a particle decreases, in world coordinates (distance
// from the projection grid plane) per second
#define WAVE_AMPLITUDE_FALLOFF 1

// When multiplied by the radius of a wave particle, yields the maximum distance (in world coordinates)
// between neighboring wave particles (i.e. before subidivision occurs)
// TODO: read the paper for the suggested value
#define WAVE_SUBDIVISION_COEFFICIENT .1

#endif // WAVECONSTANTS_H
