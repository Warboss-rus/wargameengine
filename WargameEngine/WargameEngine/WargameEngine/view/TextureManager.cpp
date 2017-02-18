#include "TextureManager.h"
#include "../LogWriter.h"
#include "../AsyncFileProvider.h"
#include "IImageReader.h"
#include "../Utils.h"
#include "../MemoryStream.h"
#include <algorithm>
#include <iterator>

void CTextureManager::UseTexture(CImage const& img, ICachedTexture& texture, int additionalFlags)
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
void ApplyTeamcolor(CImage & image, std::wstring const& maskFile, unsigned char * color, std::wstring const& fileName);

std::unique_ptr<ICachedTexture> CTextureManager::LoadTexture(std::wstring const& path, std::vector<sTeamColor> const& teamcolor, bool now, int flags)
{
	std::unique_ptr<ICachedTexture> tex = m_helper.CreateEmptyTexture();
	ICachedTexture& texRef = *tex;
	sReaderParameters params;
	params.flipBmp = m_helper.ForceFlipBMP();
	params.force32bit = m_helper.Force32Bits();
	params.convertBgra = m_helper.ConvertBgra();
	std::shared_ptr<CImage> img = std::make_shared<CImage>();
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
						for (auto& color: teamcolor)
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
		}
	}, [=, &texRef]() {
		UseTexture(*img, texRef, flags);
	}, [](std::exception const& e) {
		LogWriter::WriteLine(e.what());
	}, now);
	return tex;
}

CTextureManager::CTextureManager(ITextureHelper & helper, CAsyncFileProvider & asyncFileProvider)
	:m_helper(helper), m_asyncFileProvider(asyncFileProvider)
{
}

CTextureManager::~CTextureManager()
{
}

void CTextureManager::SetTexture(std::wstring const& path, int flags)
{
	if(path.empty()) 
	{
		m_helper.UnbindTexture();
		return;	
	}
	auto pair = std::make_pair(path, std::vector<sTeamColor>());
	if(m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(path, pair.second, false, flags);
	}
	m_helper.SetTexture(*m_textures[pair]);
}

std::unique_ptr<ICachedTexture> CTextureManager::CreateCubemapTexture(std::wstring const& right, std::wstring const& left, std::wstring const& back, std::wstring const& front, std::wstring const& top, std::wstring const& bottom, int flags /*= 0*/)
{
	std::unique_ptr<ICachedTexture> tex = m_helper.CreateEmptyTexture();
	ICachedTexture& texRef = *tex;
	sReaderParameters params;
	params.flipBmp = m_helper.ForceFlipBMP();
	params.force32bit = m_helper.Force32Bits();
	params.convertBgra = m_helper.ConvertBgra();
	const std::vector<std::wstring> imagePaths = { right, left, back, front, top, bottom };
	std::shared_ptr<std::vector<CImage>> images = std::make_shared<std::vector<CImage>>(6);
	std::shared_ptr<size_t> imagesReady = std::make_shared<size_t>(0);
	for (size_t i = 0; i < imagePaths.size(); ++i)
	{
		std::wstring path = imagePaths[i];
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
			}
		}, [=, &texRef]() {
			++(*imagesReady);
			if (*imagesReady == images->size())
			{
				std::vector<sTextureMipMap> sides;
				std::transform(images->begin(), images->end(), std::back_inserter(sides), [](CImage const& img) {
					return sTextureMipMap{img.GetData(), static_cast<unsigned int>(img.GetWidth()), static_cast<unsigned int>(img.GetHeight()), 0};
				});
				m_helper.UploadCubemap(texRef, sides, images->front().GetBPP(), images->front().GetFlags() | flags);
			}
		}, [](std::exception const& e) {
			LogWriter::WriteLine(e.what());
		});
	}
	return tex;
}

void CTextureManager::SetAnisotropyLevel(float level)
{
	for (auto i = m_textures.begin(); i != m_textures.end(); ++i)
	{
		m_helper.SetTexture(*i->second);
		m_helper.SetTextureAnisotropy(level);
	}
	m_helper.UnbindTexture();
	m_anisotropyLevel = level;
}

void CTextureManager::LoadTextureNow(std::wstring const& path, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags)
{
	auto pair = std::pair<std::wstring, std::vector<sTeamColor>>(path, (teamcolor) ? *teamcolor : std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(path, pair.second, true, flags);
	}
}

void CTextureManager::Reset()
{
	m_textures.clear();
}

void CTextureManager::RegisterImageReader(std::unique_ptr<IImageReader> && reader)
{
	m_imageReaders.push_back(std::move(reader));
}

ICachedTexture* CTextureManager::GetTexturePtr(std::wstring const& texture)
{
	auto pair = std::pair<std::wstring, std::vector<sTeamColor>>(texture, std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(texture, pair.second, false);
	}
	return m_textures[pair].get();
}

void CTextureManager::SetTexture(std::wstring const& path, TextureSlot slot, const std::vector<sTeamColor> * teamcolor, int flags)
{
	if (path.empty())
	{
		m_helper.UnbindTexture(slot);
		return;
	}
	auto pair = std::make_pair(path, teamcolor ? *teamcolor : std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(path, pair.second, false, flags);
	}
	m_helper.SetTexture(*m_textures[pair], slot);
}

void ApplyTeamcolor(CImage & image, std::wstring const& maskFile, unsigned char * color, std::wstring const& fileName)
{
	std::wstring path = fileName.substr(0, fileName.find_last_of('.')) + maskFile + L".bmp";
	std::vector<char> maskData = ReadFile(path);
	if (maskData.empty())
	{
		LogWriter::WriteLine(L"Texture manager: Cannot open mask file " + path);
		return;
	}
	CReadMemoryStream stream(maskData.data());
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
		LogWriter::WriteLine(L"Texture manager: Mask file is not greyscale " + maskFile);
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