#include "pch.h"
#include "resourcestore.h"
#include <extensions/Paths.h>

ResourceStore::ResourceStore(const char* text, eResourceType type, ImVec2 imageSize)
    : m_ImageSize(imageSize), m_Type(type), m_FileName(text)
{
    if (m_Type != eResourceType::TYPE_IMAGE)
    {
        m_pData = std::make_unique<DataStore>(text);

        if (m_Type != eResourceType::TYPE_IMAGE_TEXT)
        {
            // Generate categories
            for (auto [k, v] : m_pData->Items())
            {
                m_Categories.push_back(std::string(k.str()));
            }
            UpdateSearchList();
        }
    }

    if (m_Type != eResourceType::TYPE_TEXT)
    {
        /*
            Textures need to be loaded from main thread
            Loading it directly here doesn't work
            TODO: 
                Maybe enabling a dx9 flag fixes this?
                Switch to initScriptsEvent
        */
        Events::initGameEvent += [text, this]()
        {
            LoadTextureResource(text);
        };
    }
}

// Get dx9 texture object from RwTexture*
static IDirect3DTexture9** GetTextureFromRaster(RwTexture* pTexture)
{
    RwRasterEx* raster = (RwRasterEx*)(&pTexture->raster->parent);

    return (&raster->m_pRenderResource->texture);
}

RwTexDictionary* LoadTexDictionary(char const* filename) {
    return plugin::CallAndReturnDynGlobal<RwTexDictionary*, char const*>(0x5B3860, filename);
}

RwTexture* ResourceStore::FindRwTextureByName(const std::string& name)
{
    for (auto& item: m_ImagesList)
    {
        if (item->m_FileName == name)
        {
            return item->m_pRwTexture;
        }
    }
    return nullptr;
}

IDirect3DTexture9** ResourceStore::FindTextureByName(const std::string& name)
{
    RwTexture *pTex = FindRwTextureByName(name);
    if (pTex)
    {
        return GetTextureFromRaster(pTex);
    }
    return nullptr;
}

void ResourceStore::LoadTextureResource(std::string&& name)
{
    std::string fullPath = PLUGIN_PATH((char*)FILE_NAME "\\") + name + ".txd";

    if (!std::filesystem::exists(fullPath))
    {
        Log::Print<eLogLevel::Warn>("Failed to load {}", fullPath);
        return;
    }

    RwTexDictionary* pRwTexDictionary = LoadTexDictionary(fullPath.c_str());

    if (pRwTexDictionary)
    {
        RwLinkList *pRLL = (RwLinkList*)pRwTexDictionary->texturesInDict.link.next;
        RwTexDictionary *pEndDic;
        bool addCategories = (m_Categories.size() < 3); // "All", "Custom"
        do
        {
            pEndDic = (RwTexDictionary*)pRLL->link.next;
            RwTexture *pTex = (RwTexture*)&pRLL[-1];

            m_ImagesList.push_back(std::make_unique<TextureResource>());
            m_ImagesList.back().get()->m_pRwTexture = pTex;

            // Fetch IDirec9Texture9* from RwTexture*
            m_ImagesList.back().get()->m_pTexture = GetTextureFromRaster(pTex);

            // Naming format in Txd `Category$TextureName`
            std::stringstream ss(pTex->name);
            std::string str;

            getline(ss, str, '$');

            if (m_Type == TYPE_TEXT_IMAGE)
            {
                // generate categories from text data
                for (auto [k, v] : m_pData->Items())
                {
                    std::string val = v.value_or<std::string>("Unknown");
                    if (val == str)
                    {
                        m_ImagesList.back().get()->m_CategoryName = k.str();
                        break;
                    }
                }
            }
            else 
            {
                m_ImagesList.back().get()->m_CategoryName = str;
            }

            if (name == "clothes")
            {
                // pass full name
                m_ImagesList.back().get()->m_FileName = pTex->name;
            }
            else
            {
                getline(ss, str, '$');
                m_ImagesList.back().get()->m_FileName = str;
            }

            // Genereate categories
            if (m_Type != TYPE_TEXT_IMAGE && 
            !std::count(m_Categories.begin(), m_Categories.end(), m_ImagesList.back().get()->m_CategoryName))
            {
                m_Categories.push_back(m_ImagesList.back().get()->m_CategoryName);
            }
            
            pRLL = (RwLinkList*)pEndDic;
        }
        while ( pEndDic != (RwTexDictionary*)&pRwTexDictionary->texturesInDict );
    }
}

void ResourceStore::UpdateSearchList(bool favourites)
{
    m_nSearchList.clear();
    if (favourites)
    {
        for (auto [key, val] : *m_pData->GetTable("Favourites"))
        {
            std::string label = std::string(key.str());
            if (m_Filter.PassFilter(label.c_str()))
            {
                std::string data = val.value_or<std::string>("Unkonwn");
                m_nSearchList.push_back({std::move(std::string("Favourites")), std::move(label), std::move(data)});
            }
        }
    }
    else
    {
        for (auto [cat, table] : m_pData->Items())
        {
            // Don't show favourites in "All"
            if (m_Selected == "All" && cat == "Favourites")
            {
                continue;
            }
            if (cat.str() == m_Selected || m_Selected == "All")
            {
                if (!table.as_table())
                {
                    return;
                }
                for (auto [key, val] : *table.as_table()->as_table())
                {
                    std::string label = std::string(key.str());
                    if (m_Filter.PassFilter(label.c_str()))
                    {
                        std::string data = val.value_or<std::string>("Unkonwn");
                        m_nSearchList.push_back({std::move(std::string(cat.str())), std::move(label), std::move(data)});
                    }
                }
            }
        }
    }
    m_nSearchList.shrink_to_fit();
}