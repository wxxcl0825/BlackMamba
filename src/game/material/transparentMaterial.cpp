#include "game/material/transparentMaterial.h"
#include "game/material/phongMaterial.h"

void TransparentMaterial::apply(const RenderInfo &info) {
  PhongMaterial::apply(info);

   _alphaMap->bind();
  _shader->setUniform("alphaMap", (int)_alphaMap->getUnit());
  _shader->setUniform("useAlphaMap", 1.0f);
}