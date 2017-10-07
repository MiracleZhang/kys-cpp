#include "UIItem.h"
#include "Save.h"
#include "others\libconvert.h"
#include "Font.h"
#include "MainScene.h"

UIItem::UIItem()
{
    item_buttons_.resize(21);
    items_.resize(ITEM_IN_BAG_COUNT);
    for (int i = 0; i < item_buttons_.size(); i++)
    {
        auto b = new Button();
        item_buttons_[i] = b;
        b->setPosition(i % item_each_line_ * 85 + 40, i / item_each_line_ * 85 + 100);
        //b->setTexture("item", Save::getInstance()->getItemByBagIndex(i)->ID);
        addChild(b);
    }
    title_ = new MenuText();
    title_->setStrings({ "劇情", "兵甲", "丹藥", "暗器", "拳經", "劍譜", "刀錄", "奇門", "心法" });
    title_->setFontSize(25);
    title_->arrange(0, 50, 65, 0);

    addChild(title_);
    int i = 0;
}

UIItem::~UIItem()
{
}

//原分类：0剧情，1装备，2秘笈，3药品，4暗器
//详细分类："0劇情", "1兵甲", "2丹藥", "3暗器", "4拳經", "5劍譜", "6刀錄", "7奇門", "8心法"
int UIItem::getItemDetailType(Item* item)
{
    if (item == nullptr) { return -1; }
    if (item->ItemType == 0)
    {
        return 0;
    }
    else if (item->ItemType == 1)
    {
        return 1;
    }
    else if (item->ItemType == 3)
    {
        return 2;
    }
    else if (item->ItemType == 4)
    {
        return 3;
    }
    else if (item->ItemType == 2)
    {
        auto m = Save::getInstance()->getMagic(item->MagicID);
        if (m)
        {
            //吸取内力类归为8
            if (m->HurtType == 0)
            {
                return m->MagicType + 3;
            }
        }
        return 8;
    }
    //未知的种类当成剧情
    return 0;
}

//注意填充到列表的是在背包中的位置而不是物品id
void UIItem::geItemBagIndex(int item_type)
{
    std::fill(items_.begin(), items_.end(), -1);
    int k = 0;
    for (int i = 0; i < ITEM_IN_BAG_COUNT; i++)
    {
        if (getItemDetailType(Save::getInstance()->getItemByBagIndex(i)) == item_type)
        {
            items_[k] = i;
            k++;
        }
    }
}

void UIItem::draw()
{
    geItemBagIndex(title_->getResult());

    Item* current_item = nullptr;
    int current_count = 0;
    result_ = -1;
    for (int i = 0; i < item_buttons_.size(); i++)
    {
        auto item = Save::getInstance()->getItemByBagIndex(items_[i]);
        if (item)
        {
            item_buttons_[i]->setTexture("item", item->ID);
            if (item_buttons_[i]->getState() == Pass)
            {
                current_item = item;
                current_count = Save::getInstance()->getItemCountByBagIndex(items_[i]);
            }
        }
        else
        {
            item_buttons_[i]->setTexture("item", -1);
        }
    }
    if (current_item)
    {
        auto str = convert::formatString("%s", current_item->Name);
        Font::getInstance()->draw(str, 25, x_ + 10, y_ + 370, { 255, 255, 255, 255 });
        str = convert::formatString("%5d", current_count);
        Font::getInstance()->draw(str, 25, x_ + 10 + 25 * 7, y_ + 370, { 255, 255, 255, 255 });
        result_ = current_item->ID;
        showItemProperty(current_item);
    }
}

void UIItem::dealEvent(BP_Event& e)
{


}

void UIItem::showItemProperty(Item* item)
{
    Font::getInstance()->draw(item->Introduction, 20, x_ + 10, y_ + 400, { 255, 255, 255, 255 });

    int x = 10, y = 430;
    int size = 20;

    //以下显示物品的属性
    BP_Color c = { 255, 228, 196, 255 };

    //特别判断罗盘
    if (item->isCompass())
    {
        int x, y;
        MainScene::getIntance()->getManPosition(x, y);
        auto str = convert::formatString("當前坐標（%d, %d）", x, y);
        showOneProperty(1, str, size, c, x, y);
    }

    //剧情物品不继续显示了
    if (item->ItemType == 0)
    {
        return;
    }

    //Font::getInstance()->draw("效果：", size, x_ + x, y_ + y, c);
    //y += size + 10;

    showOneProperty(item->AddHP, "生命%+d", size, c, x, y);
    showOneProperty(item->AddMaxHP, "生命上限%+d", size, c, x, y);
    showOneProperty(item->AddMP, "內力%+d", size, c, x, y);
    showOneProperty(item->AddMaxMP, "內力上限%+d", size, c, x, y);
    showOneProperty(item->AddPhysicalPower, "體力%+d", size, c, x, y);
    showOneProperty(item->AddPoison, "中毒%+d", size, c, x, y);

    showOneProperty(item->AddAttack, "攻擊%+d", size, c, x, y);
    showOneProperty(item->AddSpeed, "輕功%+d", size, c, x, y);
    showOneProperty(item->AddDefence, "防禦%+d", size, c, x, y);

    showOneProperty(item->AddMedcine, "醫療%+d", size, c, x, y);
    showOneProperty(item->AddUsePoison, "用毒%+d", size, c, x, y);
    showOneProperty(item->AddDetoxification, "解毒%+d", size, c, x, y);
    showOneProperty(item->AddAntiPoison, "抗毒%+d", size, c, x, y);

    showOneProperty(item->AddFist, "拳掌%+d", size, c, x, y);
    showOneProperty(item->AddSword, "御劍%+d", size, c, x, y);
    showOneProperty(item->AddKnife, "耍刀%+d", size, c, x, y);
    showOneProperty(item->AddUnusual, "特殊兵器%+d", size, c, x, y);
    showOneProperty(item->AddHiddenWeapon, "暗器%+d", size, c, x, y);

    showOneProperty(item->AddKnowledge, "作弊%+d", size, c, x, y);
    showOneProperty(item->AddMorality, "道德%+d", size, c, x, y);
    showOneProperty(item->AddAttackWithPoison, "攻擊帶毒%+d", size, c, x, y);

    showOneProperty(item->ChangeMPType == 2, "內力調和", size, c, x, y);
    showOneProperty(item->AddAttackTwice == 1, "雙擊", size, c, x, y);

    auto magic = Save::getInstance()->getMagic(item->MagicID);
    if (magic)
    {
        auto str = convert::formatString("習得武學%s", magic->Name);
        showOneProperty(1, str, size, c, x, y);
    }

    //以下显示物品需求

    //药品和暗器类不继续显示了
    if (item->ItemType == 3 || item->ItemType == 4)
    {
        return;
    }

    x = 10;
    y += size + 10;  //换行
    c = { 230, 230, 250, 255 };
    //Font::getInstance()->draw("需求：", size, x_ + x, y_ + y, c);
    //y += size + 10;
    auto role = Save::getInstance()->getRole(item->OnlySuitableRole);
    if (role)
    {
        auto str = convert::formatString("僅適合%s", role->Name);
        showOneProperty(1, str, size, c, x, y);
        return;
    }

    showOneProperty(item->NeedMP, "內力%d", size, c, x, y);
    showOneProperty(item->NeedAttack, "攻擊%d", size, c, x, y);
    showOneProperty(item->NeedSpeed, "輕功%d", size, c, x, y);

    showOneProperty(item->NeedMedcine, "醫療%d", size, c, x, y);
    showOneProperty(item->NeedUsePoison, "用毒%d", size, c, x, y);
    showOneProperty(item->NeedDetoxification, "解毒%d", size, c, x, y);

    showOneProperty(item->NeedFist, "拳掌%d", size, c, x, y);
    showOneProperty(item->NeedSword, "御劍%d", size, c, x, y);
    showOneProperty(item->NeedKnife, "耍刀%d", size, c, x, y);
    showOneProperty(item->NeedUnusual, "特殊兵器%d", size, c, x, y);
    showOneProperty(item->NeedHiddenWeapon, "暗器%d", size, c, x, y);

    showOneProperty(item->NeedIQ, "資質%d", size, c, x, y);

    showOneProperty(item->NeedExp, "基礎經驗%d", size, c, x, y);
}

void UIItem::showOneProperty(int v, std::string format_str, int size, BP_Color c, int& x, int& y)
{
    if (v != 0)
    {
        auto str = convert::formatString(format_str.c_str(), v);
        //测试是不是出界了
        int draw_length = size * str.size() / 2 + size;
        int x1 = x + draw_length;
        if (x1 > 700)
        {
            x = 10;
            y += size + 5;
        }
        Font::getInstance()->draw(str, size, x_ + x, y_ + y, c);
        x += draw_length;
    }
}
