#include "Graphics/Mesh.h"
#include "Graphics/Vertex.h"

void Mesh::CreateBuffers(ID3D11Device* device)
{
    if (!device) return;

    if (vertices.empty() || indices.empty())
    {
        // nothing to create
        return;
    }
    // VertexBuffer
    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(
        &vbDesc,
        &vbData,
        vertexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugStringA("CreateBuffer(Vertex) FAILED\n");
        return;
    }


    // IndexBuffer
    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(uint32_t) * static_cast<UINT>(indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = indices.data();

    hr = device->CreateBuffer(
        &ibDesc,
        &ibData,
        indexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        OutputDebugStringA("CreateBuffer(Index) FAILED\n");
        return;
    }

}

