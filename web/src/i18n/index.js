import Vue from 'vue'
import VueI18n from 'vue-i18n'
import elementEnLocale from 'element-ui/lib/locale/lang/en' // element-ui lang
import elementZhCnLocale from 'element-ui/lib/locale/lang/zh-CN'// element-ui lang
import elementEsLocale from 'element-ui/lib/locale/lang/es'// element-ui lang
import elementZhTwLocale from 'element-ui/lib/locale/lang/zh-TW'// element-ui lang
import elementRuLocale from 'element-ui/lib/locale/lang/ru-RU'// element-ui lang
import enLocale from './en_US'
import zhTWLocale from './zh_TW'
import zhCNLocale from './zh_Hans'
import esLocale from './es_ES'
import ruLocale from "./ru_RU";


Vue.use(VueI18n)

const messages = {
  en: {
    ...enLocale,
    ...elementEnLocale
  },
  zh_TW: {
    ...zhTWLocale,
    ...elementZhTwLocale
  },
  es: {
    ...esLocale,
    ...elementEsLocale
  },
  zh_CN: {
    ...zhCNLocale,
    ...elementZhCnLocale
  },
  ru_RU: {
    ... ruLocale,
    ... elementRuLocale
  }
}
export function getLanguage() {
  // if has not choose language
  const language = (navigator.language || navigator.browserLanguage).toLowerCase()
  const locales = Object.keys(messages)
  for (const locale of locales) {
    if (language.indexOf(locale) > -1) {
      //return 'en'
      return locale
    }
  }
  return 'zh_TW'
}
const i18n = new VueI18n({
  // set locale
  // options: en | zh | es
  locale: getLanguage(),
  // set locale messages
  messages
})

export default i18n
