#include "pch.h"
#include "Collider.h"

/*DirectX::SimpleMath::Vector2 Collider::ProcessCollision(Collider* a, Transform aTrans, Collider* b, Transform bTrans)
{
    if (a->isTrigger || b->isTrigger)
        return DirectX::SimpleMath::Vector2();
    if (a->getCompId() == ComponentType::SQUARE_COLLLIDER && b->getCompId() == ComponentType::SQUARE_COLLLIDER)
    {
		DirectX::XMVECTOR myPos = DirectX::XMVectorAdd(aTrans->GetPosXMVector(), DirectX::XMLoadFloat4(&center));
		DirectX::XMVECTOR toCheckPos = DirectX::XMVectorAdd(toCheck->getComponent<Transform>()->GetPosXMVector(), DirectX::XMLoadFloat4(&toCheckCollider->center));

		DirectX::XMFLOAT2 myPosf;
		DirectX::XMStoreFloat2(&myPosf, myPos);

		DirectX::XMFLOAT2 toCheckPosf;
		DirectX::XMStoreFloat2(&toCheckPosf, toCheckPos);
		DirectX::SimpleMath::Vector2 delta;
		DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(myPos, toCheckPos);
		difference.m128_f32[0] = fabs(difference.m128_f32[0]) - ((width + toCheckCollider->width) / 2);
		difference.m128_f32[1] = fabs(difference.m128_f32[1]) - ((height + toCheckCollider->height) / 2);
		if (difference.m128_f32[1] > difference.m128_f32[0])
		{
			if (myPos.m128_f32[1] < toCheckPos.m128_f32[1])
			{
				delta = DirectX::SimpleMath::Vector2(0.0f, -difference.m128_f32[1]);
			}
			else
			{
				delta = DirectX::SimpleMath::Vector2(0.0f, difference.m128_f32[1]);
			}
		}
		else
		{
			if (myPos.m128_f32[0] < toCheckPos.m128_f32[0])
			{
				delta = DirectX::SimpleMath::Vector2(-difference.m128_f32[0], 0);
			}
			else
			{
				delta = DirectX::SimpleMath::Vector2(difference.m128_f32[0], 0);
			}
		}
		if (difference.m128_f32[1] > 0.1 || difference.m128_f32[1] < 0.1)
		{
			int a = 0;
		}
    }
    else if(a->getCompId() == ComponentType::CIRCLE_COLLIDER && b->getCompId() == ComponentType::CIRCLE_COLLIDER)
    {
        //circle x circle collision
    }
    else if ((a->getCompId() == ComponentType::SQUARE_COLLLIDER || b->getCompId() == ComponentType::SQUARE_COLLLIDER) && (a->getCompId() == ComponentType::CIRCLE_COLLIDER || b->getCompId() == ComponentType::CIRCLE_COLLIDER))
    {
        //circle x square collision
    }
    return DirectX::SimpleMath::Vector2();
}*/
