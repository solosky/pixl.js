# Amiidb 

## ⾸⻚

* 浏览..
* 搜索..
* 我的收藏..
* 我的数据..
* 设置..
* [退出]

## 浏览游戏列表

* 塞尔达传说
* 动物森友会
* 怪物猎⼈:崛起
* …
* [返回]


## Amiibo列表

* *林克
* ⽶法
* 塞尔达
* 加农
* [上⼀⻚]
* [下⼀⻚]
* [返回]

```
备注：
动森数据⽐较多的情况会分⻚展示
*为已收藏标记
如何做当天已使⽤过？？
```

## 列表菜单

* 收藏..
* 保存为..
* 搜索..
* [返回]


## Amiibo详情
* <01 ab:bc:cd:de:ef:f0:01>
* Link 
* 宝⽯ 攀爬套

## Amiibo详情菜单

* 随机⽣成
* ⾃动随机 [开]
* 收藏..
* 取消收藏…
* 新建amiibo…
* 保存为..
* [返回列表]
* [返回菜单]
* [退出应⽤]


## 搜索

* [________________]
* Link/塞尔达传说
*Link/⼤乱⽃
* [返回菜单]


## 我的收藏
* 收藏夹1
* 收藏夹2
* [返回菜单]


## 我的收藏菜单

* 新建收藏夹…
* 删除收藏夹…
* 清空收藏夹…
* [返回]

## 收藏列表⻚

```
和amiibo列表⻚⼀致
点击amiibo跳amiibo详情⻚
```

## 我的数据

* 00-link
* 01-8bitlink
* …..
* 20-空数据
* [返回菜单]

```
备注: 20个槽位，随意保存，可以在设置⾥修改槽位数量
```

## 设置

* Amiibo keys [已加载]
* 数据槽位 [20]
* [返回菜单]

## 数据结构

1.Amiibo: amiibo记录，包含id ，游戏系列，等信息，
Amiibo id, amiibo key, name, character, game series
2.Game: 以游戏纬度，组织游戏数据
Game id, name,amiibo cnt
3. Link: 游戏和amiibo之间的关联关系，掉落备等
Link id, amiibo id, game id, usage
4.Fav: 收藏记录
Fav id, Amiibo id, game id
5.FavFolder: 收藏夹记录，可以通过文件夹记录
6.Figure: 用户保存的游戏数据
7.i18n: 翻译记录
I18n id, obj type, obj id, lang, text
8.Avatar: 图片记录
Amiibo id, tag, image data


收藏实现方式一
每个收藏夹对应一个文件夹
每个收藏创建一个amiibo key对应的文件

用户保存游戏数据实现方式
固定一个文件夹，每个数据文件按ntag-xx.bin的方式保存

做一个web界面用户更新数据？