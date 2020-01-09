# EMS-ESP Documentation
This is the documentation for EMS-ESP. Thanks to Tasmota for providing examples.

## Editing Files

Use normal markdown syntax with some enhancements, see below.

### Disable click on zoom for an image
Images support click on zoom function. If you don't want to use it:
```
![](image.png ":no-zoom")
```
### Image resizing
Due to click on zoom you can directly link to a big image but make it render smaller on the page:

```
![logo](https://docsify.js.org/_media/icon.svg ':size=50x100')
![logo](https://docsify.js.org/_media/icon.svg ':size=100')

<!-- Supports percentage -->

![logo](https://docsify.js.org/_media/icon.svg ':size=10%')
```

### Rendering Alert Tags

Blockquotes `>` can now look even fancier:

?> =`?>`

!> = `!>`

> [!TIP]
> `> [!TIP]`

> [!DANGER]
> `> [!DANGER]`

> [!NOTE]
> `> [!NOTE]`

> [!EXAMPLE]
> `> [!EXAMPLE]`

> [!DANGER|style:flat]
> `> [!DANGER|style:flat]`

### Creating Tabs

```
<!-- tabs:start -->

#### ** First Tab Title **

Hello!

#### ** Second Tab Title **

Bonjour!

#### ** Third Tab Title **

Ciao!

<!-- tabs:end -->
```



