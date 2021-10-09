#include "Renderer.h"

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Math/Color.h>
#include <MagnumPlugins/StlImporter/importStaticPlugin.cpp>
#include <cassert>

using namespace Corrade;
using namespace Magnum;
using namespace Magnum::Math::Literals;

class PhongDrawable : public Magnum::SceneGraph::Drawable3D {
public:
    explicit PhongDrawable(Object3D& object, Magnum::Shaders::Phong& shader,
                           Magnum::GL::Mesh& mesh, Magnum::SceneGraph::DrawableGroup3D& drawables)
        : Magnum::SceneGraph::Drawable3D{object, &drawables}
        , _shader(shader)
        , _mesh(mesh)
    {
    }

    void draw(const Magnum::Matrix4& transformation, Magnum::SceneGraph::Camera3D& camera) {
        _shader.setTransformationMatrix(transformation);
        _shader.setNormalMatrix(transformation.normalMatrix());
        _shader.setProjectionMatrix(camera.projectionMatrix());
        _shader.draw(_mesh);
    }

private:
    Magnum::Shaders::Phong& _shader;
    Magnum::GL::Mesh& _mesh;
};

Renderer::Renderer(Scene3D& scene, Magnum::Vector2i size) : _scene(scene),
    _camera(scene, Vector3::zAxis(110.0f), Vector3(), Vector3::yAxis(), 45.0_degf, size, size){
    _phongShaders[0] = Magnum::Shaders::Phong{{}, 4};
    _phongShaders[0].setDiffuseColor(0xec9993_rgbf)
        .setShininess(1000.0f)
        .setLightColors({0x444444_rgbf, 0x444444_rgbf, 0x444444_rgbf, 0x444444_rgbf})
        .setLightPositions({{-5.0f, 0.0f, 5.0f}, {5.f, 0.0f, 0.0f}, {-5.0f, 0.0f, 0.0f}, {0.0f, 5.0f, 0.0f}});
    _phongShaders[1] = Magnum::Shaders::Phong{{}, 4};
    _phongShaders[1].setDiffuseColor(0xeaddd9_rgbf)
        .setShininess(2000.0f)
        .setLightColors({0x444444_rgbf, 0x444444_rgbf, 0x444444_rgbf, 0x444444_rgbf})
        .setLightPositions({{-5.0f, 0.0f, 5.0f}, {5.f, 0.0f, 0.0f}, {-5.0f, 0.0f, 0.0f}, {0.0f, 5.0f, 0.0f}});

    _offscreen.init(size);
}

bool Renderer::loadStl(const char* path) {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer =
        manager.loadAndInstantiate("StlImporter");
    CORRADE_INTERNAL_ASSERT(importer);
    if(!importer->openFile(path))
        Fatal{} << "Can't open *.stl with StlImporter";

    _data = importer->mesh(0);

    /* Check that we have at least positions and normals */
    _meshOrigin.setPrimitive(_data->primitive());
    if(!_data->hasAttribute(Trade::MeshAttribute::Position) ||
       !_data->hasAttribute(Trade::MeshAttribute::Normal))
        Fatal{} << "Oh well";

    /* Interleave vertex data */
    _verticesOrigin.resize(_data->vertexCount());
    for(int i = 0; i < _data->vertexCount(); ++i) {
        const Vector3& position = _data->attribute<Vector3>(Trade::MeshAttribute::Position)[i];
        const Vector3& normal = _data->attribute<Vector3>(Trade::MeshAttribute::Normal)[i];
        _verticesOrigin[i] = Vertex{position, normal};
    }
    _vbuffer.setData({_verticesOrigin.data(), _verticesOrigin.size()});
    _meshOrigin.addVertexBuffer(_vbuffer, 0,
                          Shaders::Phong::Position{}, Shaders::Phong::Normal{});

    /* Set up an index buffer, if the mesh is indexed*/
    if(_data->isIndexed()) {
        GL::Buffer indices;
        indices.setData(_data->indicesAsArray());
        _meshOrigin.setIndexBuffer(std::move(indices), 0, MeshIndexType::UnsignedInt)
            .setCount(_data->indexCount());
    } else _meshOrigin.setCount(_data->vertexCount());

    auto obj3d = new Object3D{&_scene};
    new PhongDrawable{*obj3d, _phongShaders[0], _meshOrigin, _groupOrigin};

    return true;
}

void Renderer::loadSegment(const std::vector<unsigned int>& indices) {
    //Corrade::Utility::Debug{} << indices;
    //_meshSegment.setPrimitive(_data->primitive());
    //_meshSegment.addVertexBuffer(_vbuffer, 0,
    //                             Shaders::Phong::Position{}, Shaders::Phong::Normal{});
    {
        assert(indices.size() * 3 == _verticesOrigin.size());
        for(size_t i = 0; i < indices.size(); ++i) {
            for(int j = 0; j < 3; ++j) {
                _verticesSegment[indices[i]].push_back(_verticesOrigin[3 * i + j]);
            }
        }
        for(int i = 0; i < 32; ++i) {
            if(_verticesSegment[i].empty())
                continue;

            GL::Buffer vbuffer;
            vbuffer.setData({_verticesSegment[i].data(), _verticesSegment[i].size()});
            _meshesSegment[i].setPrimitive(Magnum::MeshPrimitive::Triangles)
                .addVertexBuffer(std::move(vbuffer), 0,Shaders::Phong::Position{}, Shaders::Phong::Normal{})
                .setCount(_verticesSegment[i].size());
            auto obj3d = new Object3D{&_scene};
            new PhongDrawable{*obj3d, _phongShaders[0 == i ? 0 : 1], _meshesSegment[i], _groupSegment};
        }
        _groupOrigin = _groupSegment;
    }
}
