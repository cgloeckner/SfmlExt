#include <SfmlExt/fader.hpp>

namespace sfext {

Fader::Fader()
	: in {[](sf::Time const & elapsed, float value) {
		return value + elapsed.asMilliseconds() / 30.f;
	}}
	, out{[](sf::Time const & elapsed, float value) {
		return value - elapsed.asMilliseconds() / 30.f;
	}} {
}

bool Fader::operator()(sf::Time const & elapsed, sf::SoundSource& sound, FadeMode mode, float max_volume) {
	float volume = sound.getVolume();
	bool done{false};
	
	switch (mode) {
		case FadeMode::In:
			volume = std::min(max_volume, in(elapsed, volume));
			done = (volume == max_volume);
			break;
			
		case FadeMode::Out:
			volume = std::max(0.f, out(elapsed, volume));
			done = (volume == 0.f);
			break;
			
		case FadeMode::None:
			done = (volume > max_volume);
			volume = std::max(volume, max_volume);
			break;
	}
	
	sound.setVolume(volume);
	return done;
}

// ---------------------------------------------------------------------------

MusicManager::Channel::Channel()
	: music{}
	, mode{sfext::FadeMode::None} {
}

MusicManager::MusicManager()
	: primary{}
	, secondary{}
	, fader{}
	, max_volume{100.f} {
}

void MusicManager::play(std::string const & filename) {
	if (primary.music.getStatus() == sf::SoundSource::Status::Stopped) {
		// play at primary channel
		primary.music.openFromFile(filename);
		primary.music.play();
		primary.music.setVolume(0.f);
		primary.mode = sfext::FadeMode::In;
		secondary.mode = sfext::FadeMode::Out;
		
	} else if (secondary.music.getStatus() == sf::SoundSource::Status::Stopped) { 
		// play at secondary channel
		secondary.music.openFromFile(filename);
		secondary.music.play();
		secondary.music.setVolume(0.f);
		secondary.mode = sfext::FadeMode::In;
		primary.mode = sfext::FadeMode::Out;
		
	} else {
		// play at channel which is quieter
		if (primary.music.getVolume() < secondary.music.getVolume()) {
			primary.music.openFromFile(filename);
			primary.music.play();
			primary.music.setVolume(0.f);
			primary.mode = sfext::FadeMode::In;
			secondary.mode = sfext::FadeMode::Out;
			
		} else {
			secondary.music.openFromFile(filename);
			secondary.music.play();
			secondary.music.setVolume(0.f);
			secondary.mode = sfext::FadeMode::In;
			primary.mode = sfext::FadeMode::Out;
		}
	}
}

void MusicManager::stop() {
	if (primary.music.getStatus() == sf::SoundSource::Status::Playing) {
		// fade-out primary channel
		primary.mode = sfext::FadeMode::Out;
	}
	
	if (secondary.music.getStatus() == sf::SoundSource::Status::Playing) {
		// fade-out secondary channel
		secondary.mode = sfext::FadeMode::Out;
	}
}

void MusicManager::update(sf::Time const & elapsed) {
	bool done1 = fader(elapsed, primary.music, primary.mode, max_volume);
	bool done2 = fader(elapsed, secondary.music, secondary.mode, max_volume);
	if (done1 && done2) {
		// stop playback of fade-out channel
		if (primary.mode == sfext::FadeMode::Out) {
			primary.music.stop();
		}
		if (secondary.mode == sfext::FadeMode::Out) {
			secondary.music.stop();
		}
		// stop fading
		primary.mode = sfext::FadeMode::None;
		secondary.mode = sfext::FadeMode::None;
	}
}

void MusicManager::setMaxVolume(float volume) {
	max_volume = volume;
	
	// cap channels' volume
	if (primary.music.getVolume() > volume) {
		primary.music.setVolume(volume);
	}
	if (secondary.music.getVolume() > volume) {
		secondary.music.setVolume(volume);
	}
	
	// make channels fade to max volume (if they're not fading, yet)
	if (primary.mode == sfext::FadeMode::None) {
		primary.mode = sfext::FadeMode::In;
	}
	if (secondary.mode == sfext::FadeMode::None) {
		secondary.mode = sfext::FadeMode::In;
	}
}

float MusicManager::getMaxVolume() const {
	return max_volume;
}

sfext::Fader& MusicManager::getFader() {
	return fader;
}

sf::Music& MusicManager::getMusic() {
	bool prim_play = (primary.music.getStatus() == sf::SoundSource::Status::Playing);
	bool sec_play = (secondary.music.getStatus() == sf::SoundSource::Status::Playing);
	
	if (prim_play && !sec_play) {
		return primary.music;
		
	} else if (!prim_play && sec_play) {
		return secondary.music;
		
	} else if (!prim_play && !sec_play) {
		return primary.music;
		
	} else {
		// both are playing - pick which is fading in
		bool prim_in = (primary.mode == sfext::FadeMode::In);
		bool sec_in = (secondary.mode == sfext::FadeMode::In);
		if (prim_in && !sec_in) {
			return primary.music;
			
		} else if (!prim_in && sec_in) {
			return secondary.music;
			
		} else if (!prim_in && !sec_in) {
			return primary.music;
			
		} else {
			// both are fading in - pick loudest
			if (primary.music.getVolume() > secondary.music.getVolume()) {
				return primary.music;
			} else {
				return secondary.music;
			}
		}
	}
}

}
