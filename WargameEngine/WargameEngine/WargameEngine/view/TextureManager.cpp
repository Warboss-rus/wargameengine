#include "TextureManager.h"
#include "../LogWriter.h"
#include "../AsyncFileProvider.h"
#include "IImageReader.h"

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
void ApplyTeamcolor(CImage & image, std::string const& maskFile, unsigned char * color, std::string const& fileName);

std::unique_ptr<ICachedTexture> CTextureManager::LoadTexture(std::string const& path, std::vector<sTeamColor> const& teamcolor, bool now, int flags)
{
	std::unique_ptr<ICachedTexture> tex = m_helper.CreateEmptyTexture();
	ICachedTexture& texRef = *tex;	
	bool force32b = m_helper.Force32Bits();
	bool forceFlip = m_helper.ForceFlipBMP();
	std::shared_ptr<CImage> img = std::make_shared<CImage>();
	m_asyncFileProvider.GetTextureAsync(path, [=](void* data, unsigned int size) {
		unsigned char* charData = reinterpret_cast<unsigned char*>(data);
		for (auto& reader : m_imageReaders)
		{
			if (reader->ImageIsSupported(charData, size, path))
			{
				try
				{
					*img = reader->ReadImage(charData, size, path, forceFlip, force32b);
					if (!img->IsCompressed())
					{
						for (auto& color: teamcolor)
						{
							ApplyTeamcolor(*img, color.suffix, const_cast<unsigned char*>(color.color), path);
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

void CTextureManager::SetTexture(std::string const& path, const std::vector<sTeamColor> * teamcolor, int flags)
{
	if(path.empty()) 
	{
		m_helper.UnbindTexture();
		return;	
	}
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, (teamcolor) ? *teamcolor : std::vector<sTeamColor>());
	if(m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(path, pair.second, false, flags);
	}
	m_textures[pair]->Bind();
}

void CTextureManager::SetAnisotropyLevel(float level)
{
	for (auto i = m_textures.begin(); i != m_textures.end(); ++i)
	{
		i->second->Bind();
		m_helper.SetTextureAnisotropy(level);
	}
	m_helper.UnbindTexture();
	m_anisotropyLevel = level;
}

void CTextureManager::LoadTextureNow(std::string const& path, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags)
{
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, (teamcolor) ? *teamcolor : std::vector<sTeamColor>());
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

void CTextureManager::SetTexture(std::string const& path, TextureSlot slot, int flags)
{
	m_helper.ActivateTextureSlot(slot);
	if (path.empty())
	{
		m_helper.UnbindTexture();
		m_helper.ActivateTextureSlot(TextureSlot::eDiffuse);
		return;
	}
	auto pair = std::pair<std::string, std::vector<sTeamColor>>(path, std::vector<sTeamColor>());
	if (m_textures.find(pair) == m_textures.end())
	{
		m_textures[pair] = LoadTexture(path, pair.second, false, flags);
	}
	m_textures[pair]->Bind();
	m_helper.ActivateTextureSlot(TextureSlot::eDiffuse);
}

void ApplyTeamcolor(CImage & image, std::string const& maskFile, unsigned char * color, std::string const& fileName)
{
	std::string path = fileName.substr(0, fileName.find_last_of('.')) + maskFile + ".bmp";
	FILE * fmask = fopen(path.c_str(), "rb");
	if (!fmask)
	{
		LogWriter::WriteLine("Texture manager: Cannot open mask file " + path);
		return;
	}
	fseek(fmask, 0L, SEEK_END);
	unsigned int maskSize = ftell(fmask);
	fseek(fmask, 0L, SEEK_SET);
	std::vector<unsigned char> maskData;
	maskData.resize(maskSize);
	fread(maskData.data(), 1, maskSize, fmask);
	fclose(fmask);
	int maskHeight = *(int*)&(maskData[0x12]);
	int maskWidth = *(int*)&(maskData[0x16]);
	short maskbpp = *(short*)&(maskData[0x1C]);
	if (image.GetFlags() & TEXTURE_HAS_ALPHA)
	{
		std::swap(color[0], color[2]);
	}
	if (maskbpp != 8)
	{
		LogWriter::WriteLine("Texture manager: Mask file is not greyscale " + maskFile);
		return;
	}
	for (unsigned int x = 0; x < image.GetWidth(); ++x)
	{
		for (unsigned int y = 0; y < image.GetHeight(); ++y)
		{
			unsigned int pos = (x * image.GetHeight() + y) * image.GetBPP() / 8;
			unsigned int maskPos = 54 + x * maskWidth / image.GetHeight() * maskHeight + y * maskHeight / image.GetHeight();
			for (unsigned int i = 0; i < 3; ++i)
			{
				image.GetData()[pos + i] = static_cast<unsigned char>(image.GetData()[pos + i] * (1.0 - maskData[maskPos] / 255.0) + color[i] * (maskData[maskPos] / 255.0));
			}
		}
	}
}