#include "TextureManager.h"
#include "../AsyncFileProvider.h"
#include "../LogWriter.h"
#include "../MemoryStream.h"
#include "../Utils.h"
#include "IImageReader.h"
#include <algorithm>
#include <iterator>

namespace wargameEngine
{
namespace view
{

void TextureManager::UseTexture(Image const& img, ICachedTexture& texture, int additionalFlags)
{
	if (img.GetMipmaps().empty())
	{
		additionalFlags |= TEXTURE_BUILD_MIPMAPS;
	}
	if (img.IsCompressed())
	{
		m_helper.UploadCompressedTexture(texture, img.GetData(), img.GetWidth(), img.GetHeight(), img.GetImageSize(), img.GetFlags() | additionalFlags, img.GetMipmaps());
	}
	else
	{
		m_helper.UploadTexture(texture, img.GetData(), img.GetWidth(), img.GetHeight(), img.GetBPP(), img.GetFlags() | additionalFlags, img.GetMipmaps());
	}
	m_helper.SetTextureAnisotropy(m_anisotropyLevel);
}
void ApplyTeamcolor(Image& image, const Path& maskFile, unsigned char* color, const Path& fileName);

std::unique_ptr<ICachedTexture> TextureManager::LoadTexture(const Path& path, std::vector<model::TeamColor> const& teamcolor, bool now, int flags)
{
	std::unique_ptr<ICachedTexture> tex = m_helper.CreateEmptyTexture();
	if (path.empty())
	{
		return tex;
	}
	ICachedTexture& texRef = *tex;
	sReaderParameters params;
	params.flipBmp = m_helper.ForceFlipBMP();
	params.force32bit = m_helper.Force32Bits();
	params.convertBgra = m_helper.ConvertBgra();
	std::shared_ptr<Image> img = std::make_shared<Image>();
	m_asyncFileProvider.GetTextureAsync(path, [=](void* data, size_t size) {
		unsigned char* charData = reinterpret_cast<unsigned char*>(data);
		for (auto& reader : m_imageReaders)
		{
			if (reader->ImageIsSupported(charData, size, path))
			{
				try
				{
					*img = reader->ReadImage(charData, size, path, params);
					if (!img->IsCompressed())
					{
						for (auto& color : teamcolor)
						{
							ApplyTeamcolor(*img, color.suffix, const_cast<unsigned char*>(color.color), m_asyncFileProvider.GetTextureAbsolutePath(path));
						}
					}
					return;
				}
				catch (std::exception const& e)
				{
					LogWriter::WriteLine(e.what());
				}
			}
		} }, [=, &texRef]() { UseTexture(*img, texRef, flags); }, [](std::exception const& e) { LogWriter::WriteLine(e.what()); }, now);
	return tex;
}

TextureManager::TextureManager(ITextureHelper& helper, AsyncFileProvider& asyncFileProvider)
	: m_helper(helper)
	, m_asyncFileProvider(asyncFileProvider)
{
}

TextureManager::~TextureManager()
{
}

std::unique_ptr<ICachedTexture> TextureManager::CreateCubemapTexture(const Path& right, const Path& left, const Path& back, const Path& front, const Path& top, const Path& bottom, int flags /*= 0*/)
{
	std::unique_ptr<ICachedTexture> tex = m_helper.CreateEmptyTexture();
	ICachedTexture& texRef = *tex;
	sReaderParameters params;
	params.flipBmp = m_helper.ForceFlipBMP();
	params.force32bit = m_helper.Force32Bits();
	params.convertBgra = m_helper.ConvertBgra();
	const std::vector<Path> imagePaths = { right, left, back, front, top, bottom };
	std::shared_ptr<std::vector<Image>> images = std::make_shared<std::vector<Image>>(6);
	std::shared_ptr<size_t> imagesReady = std::make_shared<size_t>(0);
	for (size_t i = 0; i < imagePaths.size(); ++i)
	{
		const Path& path = imagePaths[i];
		m_asyncFileProvider.GetTextureAsync(path, [images, this, path, params, i](void* data, size_t size) {
			unsigned char* charData = reinterpret_cast<unsigned char*>(data);
			for (auto& reader : m_imageReaders)
			{
				if (reader->ImageIsSupported(charData, size, path))
				{
					try
					{
						images->at(i) = reader->ReadImage(charData, size, path, params);
						images->at(i).StoreData();
						return;
					}
					catch (std::exception const& e)
					{
						LogWriter::WriteLine(e.what());
					}
				}
			} }, [=, &texRef]() {
			++(*imagesReady);
			if (*imagesReady == images->size())
			{
				std::vector<TextureMipMap> sides;
				std::transform(images->begin(), images->end(), std::back_inserter(sides), [](Image const& img) {
					return TextureMipMap{ img.GetData(), static_cast<unsigned int>(img.GetWidth()), static_cast<unsigned int>(img.GetHeight()), 0 };
				});
				m_helper.UploadCubemap(texRef, sides, images->front().GetBPP(), images->front().GetFlags() | flags);
			} }, [](std::exception const& e) { LogWriter::WriteLine(e.what()); });
	}
	return tex;
}

void TextureManager::SetAnisotropyLevel(float level)
{
	for (auto i = m_textures.begin(); i != m_textures.end(); ++i)
	{
		m_helper.SetTexture(*i->second);
		m_helper.SetTextureAnisotropy(level);
	}
	m_anisotropyLevel = level;
}

void TextureManager::LoadTextureNow(const Path& path, int flags)
{
	if (m_textures.find(path) == m_textures.end())
	{
		m_textures.emplace(std::make_pair(path,LoadTexture(path, std::vector<model::TeamColor>(), true, flags)));
	}
}

void TextureManager::Reset()
{
	m_textures.clear();
}

void TextureManager::RegisterImageReader(std::unique_ptr<IImageReader>&& reader)
{
	m_imageReaders.push_back(std::move(reader));
}

ICachedTexture* TextureManager::GetTexturePtr(const Path& texture, const std::vector<model::TeamColor>* teamcolor, int flags)
{
	if (teamcolor)
	{
		auto pair = std::pair<Path, std::vector<model::TeamColor>>(texture, *teamcolor);
		auto it = m_teamcolorTextures.find(pair);
		if (it == m_teamcolorTextures.end())
		{
			it = m_teamcolorTextures.emplace(std::make_pair(pair, LoadTexture(texture, pair.second, false, flags))).first;
		}
		return it->second.get();
	}
	auto it = m_textures.find(texture);
	if (it == m_textures.end())
	{
		it = m_textures.emplace(texture, LoadTexture(texture, std::vector<model::TeamColor>(), false, flags)).first;
	}
	return it->second.get();
}

void ApplyTeamcolor(Image& image, const Path& maskFile, unsigned char* color, const Path& fileName)
{
	Path path = fileName.native().substr(0, fileName.native().find_last_of('.')) + maskFile.native() + L".bmp";
	std::vector<char> maskData = ReadFile(path);
	if (maskData.empty())
	{
		LogWriter::WriteLine("Texture manager: Cannot open mask file " + path.string());
		return;
	}
	ReadMemoryStream stream(maskData.data());
	stream.Seek(0x12);
	int maskHeight = stream.ReadInt();
	int maskWidth = stream.ReadInt();
	short maskbpp = stream.ReadShort();
	if (image.GetFlags() & TEXTURE_HAS_ALPHA)
	{
		std::swap(color[0], color[2]);
	}
	if (maskbpp != 8)
	{
		LogWriter::WriteLine("Texture manager: Mask file is not greyscale " + path.string());
		return;
	}
	for (size_t x = 0; x < image.GetWidth(); ++x)
	{
		for (size_t y = 0; y < image.GetHeight(); ++y)
		{
			size_t pos = (x * image.GetHeight() + y) * image.GetBPP() / 8;
			size_t maskPos = 54 + x * maskWidth / image.GetHeight() * maskHeight + y * maskHeight / image.GetHeight();
			for (size_t i = 0; i < 3; ++i)
			{
				image.GetData()[pos + i] = static_cast<unsigned char>(image.GetData()[pos + i] * (1.0 - maskData[maskPos] / 255.0) + color[i] * (maskData[maskPos] / 255.0));
			}
		}
	}
}
}
}