#include "texture_loader.h"
#include "../logger/logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <DDSTextureLoader.h>
#include <wrl/client.h>

#include <memory>

namespace {

bool prepare_outputs(REX::W32::ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    if (!out_srv || !out_width || !out_height) {
        return false;
    }
    *out_srv = nullptr;
    *out_width = 0;
    *out_height = 0;
    return true;
}

}  // namespace

namespace SpellHotbar::TextureLoader {
    bool fromDDSFile(const std::filesystem::path & path, REX::W32::ID3D11ShaderResourceView** out_srv, int* out_width,
        int* out_height) {
        if (!prepare_outputs(out_srv, out_width, out_height)) {
            logger::error("DDS texture loading requires non-null output pointers");
            return false;
        }

        auto* renderer = RE::BSGraphics::Renderer::GetSingleton();
        if (!renderer || !renderer->data.forwarder || !renderer->data.context) {
            logger::error("Cannot find Skyrim Renderer. texture loading failed");
            return false;
        }

        auto* device = reinterpret_cast<ID3D11Device*>(renderer->data.forwarder);
        auto* context = reinterpret_cast<ID3D11DeviceContext*>(renderer->data.context);
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
        auto result = DirectX::CreateDDSTextureFromFile(
            device, context, path.wstring().c_str(), nullptr, view.GetAddressOf());
        if (FAILED(result)) {
            logger::error("Creating DDS '{}' returned 0x{:08X}", path.string(), static_cast<std::uint32_t>(result));
            return false;
        }

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        view->GetResource(resource.GetAddressOf());
        if (!resource) {
            logger::error("DDS '{}' produced no D3D resource", path.string());
            return false;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        result = resource.As(&texture);
        if (FAILED(result) || !texture) {
            logger::error("DDS '{}' resource is not a Texture2D (0x{:08X})",
                path.string(), static_cast<std::uint32_t>(result));
            return false;
        }

        D3D11_TEXTURE2D_DESC desc{};
        texture->GetDesc(&desc);
        *out_width = static_cast<int>(desc.Width);
        *out_height = static_cast<int>(desc.Height);
        *out_srv = reinterpret_cast<REX::W32::ID3D11ShaderResourceView*>(view.Detach());
        return true;
    }

    // Simple helper function to load an image into a DX11 texture with common settings
    bool fromFile(const char* filename, REX::W32::ID3D11ShaderResourceView** out_srv, int* out_width,
                             int* out_height) {
        if (!prepare_outputs(out_srv, out_width, out_height)) {
            logger::error("Image texture loading requires non-null output pointers");
            return false;
        }

        auto* renderer = RE::BSGraphics::Renderer::GetSingleton();
        if (!renderer || !renderer->data.forwarder) {
            logger::error("Cannot find Skyrim Renderer. texture loading failed");
            return false;
        }
        auto* device = reinterpret_cast<ID3D11Device*>(renderer->data.forwarder);

        // Load from disk into a raw RGBA buffer
        int image_width = 0;
        int image_height = 0;
        std::unique_ptr<unsigned char, decltype(&stbi_image_free)> image_data(
            stbi_load(filename, &image_width, &image_height, nullptr, 4), &stbi_image_free);
        if (!image_data) {
            logger::error("stbi_load failed for '{}'", filename);
            return false;
        }

        // Create texture
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = static_cast<UINT>(image_width);
        desc.Height = static_cast<UINT>(image_height);
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA sub_resource{};
        sub_resource.pSysMem = image_data.get();
        sub_resource.SysMemPitch = desc.Width * 4;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        auto result = device->CreateTexture2D(&desc, &sub_resource, texture.GetAddressOf());
        if (FAILED(result)) {
            logger::error("CreateTexture2D failed for '{}' with 0x{:08X}",
                filename, static_cast<std::uint32_t>(result));
            return false;
        }

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = desc.MipLevels;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
        result = device->CreateShaderResourceView(texture.Get(), &srv_desc, view.GetAddressOf());
        if (FAILED(result)) {
            logger::error("CreateShaderResourceView failed for '{}' with 0x{:08X}",
                filename, static_cast<std::uint32_t>(result));
            return false;
        }

        *out_width = image_width;
        *out_height = image_height;
        *out_srv = reinterpret_cast<REX::W32::ID3D11ShaderResourceView*>(view.Detach());

        return true;
    }
}
