import Vue from 'vue'
import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
import App from './App.vue'

import i18n from './i18n' // internationalization
import cookies from "vue-cookie"

Vue.use(ElementUI, {
  i18n: (key, value) => i18n.t(key, value)
})

Vue.use(cookies)

new Vue({
  el: '#app',
  i18n,
  render: h => h(App)
})
