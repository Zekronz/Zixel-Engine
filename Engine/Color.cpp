#include "Engine/ZixelPCH.h"
#include "Engine/Color.h"
#include "Engine/ZixelMacros.h"
#include "Engine/StringHelper.h"
#include "Engine/Math.h"

namespace Zixel {

	Color3 Color::toColor3(Color3f _color) {
		return { (unsigned char)(_color.r * 255.f), (unsigned char)(_color.g * 255.f), (unsigned char)(_color.b * 255.f) };
	}

	Color3f Color::toColor3f(Color3 _color) {
		return { (f32)_color.r / 255.0f, (f32)_color.g / 255.0f, (f32)_color.b / 255.0f };
	}

	Color4 Color::toColor4(Color4f _color) {
		return { (unsigned char)(_color.r * 255.f), (unsigned char)(_color.g * 255.f), (unsigned char)(_color.b * 255.f), (unsigned char)(_color.a * 255.f) };
	}

	Color4f Color::toColor4f(Color4 _color) {
		return { (f32)_color.r / 255.0f, (f32)_color.g / 255.0f, (f32)_color.b / 255.0f, (f32)_color.a / 255.0f };
	}

	bool Color::match(Color4 _color1, Color4 _color2) {
		return (_color1.r == _color2.r && _color1.g == _color2.g && _color1.b == _color2.b && _color1.a == _color2.a);
	}

	Color3 Color::HSVToRGB(Color3 _hsv) {

		if (_hsv.g <= 0) {
			return { _hsv.b, _hsv.b, _hsv.b };
		}

		f64 hue = ((f64)_hsv.r / 255.0) * 360;
		f64 sat = ((f64)_hsv.g / 255.0);
		f64 val = ((f64)_hsv.b / 255.0);

		f64 c = sat * val;
		f64 x = c * (1.0 - fabs(fmod(hue / 60.0, 2.0) - 1.0));
		f64 m = val - c;

		f64 r, g, b;

		if (hue >= 0 && hue < 60) {

			r = c;
			g = x;
			b = 0;

		}
		else if (hue >= 60 && hue < 120) {

			r = x;
			g = c;
			b = 0;

		}
		else if (hue >= 120 && hue < 180) {

			r = 0;
			g = c;
			b = x;

		}
		else if (hue >= 180 && hue < 240) {

			r = 0;
			g = x;
			b = c;

		}
		else if (hue >= 240 && hue < 300) {

			r = x;
			g = 0;
			b = c;

		}
		else {

			r = c;
			g = 0;
			b = x;

		}

		return { (u8)round((r + m) * 255.0), (u8)round((g + m) * 255.0), (u8)round((b + m) * 255.0) };

	}

	Color3 Color::RGBToHSV(Color3 _rgb) {

		f64 r = (f64)_rgb.r / 255.0;
		f64 g = (f64)_rgb.g / 255.0;
		f64 b = (f64)_rgb.b / 255.0;

		u8 channelMin = (_rgb.r < _rgb.g) ? (_rgb.r < _rgb.b ? _rgb.r : _rgb.b) : (_rgb.g < _rgb.b ? _rgb.g : _rgb.b);
		u8 channelMax = (_rgb.r > _rgb.g) ? (_rgb.r > _rgb.b ? _rgb.r : _rgb.b) : (_rgb.g > _rgb.b ? _rgb.g : _rgb.b);
			
		u8 delta = channelMax - channelMin;

		f64 h = 0.0, s = 0.0, v = 0.0;

		f64 dMin = (f64)channelMin / 255.0;
		f64 dMax = (f64)channelMax / 255.0;

		if (delta > 0) {

			f64 dDelta = (dMax - dMin);

			if (channelMax == _rgb.r) {
				h = 60.0 * (fmod(((g - b) / dDelta), 6.0));
			}
			else if (channelMax == _rgb.g) {
				h = 60.0 * (((b - r) / dDelta) + 2.0);
			}
			else {
				h = 60.0 * (((r - g) / dDelta) + 4.0);
			}

			if (channelMax > 0) {
				s = (dDelta / dMax);
			}	

		}

		v = dMax;

		if (h < 0.0) {
			h = 360.0 + h;
		}

		return { (u8)round((h / 360.0) * 255.0), (u8)round(s * 255.0), (u8)round(v * 255.0) };

	}

	std::string Color::RGBToHex(Color3 _rgb) {

		std::string hex = "0123456789ABCDEF";

		std::string output;
		output += hex[(_rgb.r >> 4) & 15];
		output += hex[_rgb.r & 15];
		output += hex[(_rgb.g >> 4) & 15];
		output += hex[_rgb.g & 15];
		output += hex[(_rgb.b >> 4) & 15];
		output += hex[_rgb.b & 15];

		return output;

	}

	Color3 Color::hexToRGB(std::string& _hex) {

		std::string filter = "0123456789ABCDEFabcdef";

		Color3 rgb;

		for (size_t i = 0; i < _hex.length(); ++i) {

			if (i >= 6) {
				break;
			}

			size_t ind = filter.find(_hex[i]);
			if (ind == filter.npos) {
				ind = 0;
			}
			else if (ind > 15) {
				ind -= 6;
			}

			if (i == 0) {
				rgb.r += ((u8)ind << 4);
			}
			else if (i == 1) {
				rgb.r += (u8)ind;
			}
			else if (i == 2) {
				rgb.g += ((u8)ind << 4);
			}
			else if (i == 3) {
				rgb.g += (u8)ind;
			}
			else if (i == 4) {
				rgb.b += ((u8)ind << 4);
			}
			else if (i == 5) {
				rgb.b += (u8)ind;
			}

		}

		return rgb;

	}

	u8 Color::RGBToLum(Color3 _rgb) {
		Color3f col = toColor3f(_rgb);
		f32 lum = (col.r * 0.2126f + col.g * 0.7152f + col.b * 0.0722f); //Basic approximation.
		return (u8)roundf(lum * 255.0f);
	}

	Color4 Color::blendColor(Color4 _source, Color4 _destination, BlendMode _blendMode) {

		if (_blendMode == BlendMode::Overwrite) return _source;

		if (_source.a <= 0) {
			return _destination;
		}

		f32 sRed = _source.r / 255.0f;
		f32 sGreen = _source.g / 255.0f;
		f32 sBlue = _source.b / 255.0f;
		f32 sAlpha = _source.a / 255.0f;

		f32 dRed = _destination.r / 255.0f;
		f32 dGreen = _destination.g / 255.0f;
		f32 dBlue = _destination.b / 255.0f;
		f32 dAlpha = _destination.a / 255.0f;

		if (_destination.a > 0) {

			switch (_blendMode) {

			case BlendMode::Multiply:

				sRed *= dRed + (1.0f - dRed) * (1.0f - dAlpha);
				sGreen *= dGreen + (1.0f - dGreen) * (1.0f - dAlpha);
				sBlue *= dBlue + (1.0f - dBlue) * (1.0f - dAlpha);

				break;

			case BlendMode::Additive:

				sRed = Math::minFloat(sRed + (dRed * dAlpha), 1.0f);
				sGreen = Math::minFloat(sGreen + (dGreen * dAlpha), 1.0f);
				sBlue = Math::minFloat(sBlue + (dBlue * dAlpha), 1.0f);

				break;

			case BlendMode::Subtractive:

				sRed = Math::maxFloat(sRed - (dRed * dAlpha), 0.0f);
				sGreen = Math::maxFloat(sGreen - (dGreen * dAlpha), 0.0f);
				sBlue = Math::maxFloat(sBlue - (dBlue * dAlpha), 0.0f);

				break;

			default:
				break;

			}

			sRed = (sRed * sAlpha + dRed * dAlpha * (1.0f - sAlpha)) / (sAlpha + dAlpha * (1.0f - sAlpha));
			sGreen = (sGreen * sAlpha + dGreen * dAlpha * (1.0f - sAlpha)) / (sAlpha + dAlpha * (1.0f - sAlpha));
			sBlue = (sBlue * sAlpha + dBlue * dAlpha * (1.0f - sAlpha)) / (sAlpha + dAlpha * (1.0f - sAlpha));

		}

		sAlpha = dAlpha + (1.0f - dAlpha) * sAlpha;

		sRed = Math::clampFloat(sRed, 0.0f, 1.0f);
		sGreen = Math::clampFloat(sGreen, 0.0f, 1.0f);
		sBlue = Math::clampFloat(sBlue, 0.0f, 1.0f);
		sAlpha = Math::clampFloat(sAlpha, 0.0f, 1.0f);

		return { (u8)(sRed * 255.0f), (u8)(sGreen * 255.0f), (u8)(sBlue * 255.0f), (u8)(sAlpha * 255.0f) };

	}

	u8 Color::subtractAlpha(u8 _source, u8 _destination) {

		if (_source == 0) {
			return _destination;
		}

		return _destination - (u8)roundf(_destination * (_source / 255.0f));

	}

}